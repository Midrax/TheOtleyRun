#include "MaskActor.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AMaskActor::AMaskActor()
{
    PrimaryActorTick.bCanEverTick = true;

    // Root
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    // Base Mesh
    BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
    BaseMesh->SetupAttachment(Root);

    // Face Root (holds all planes)
    FaceRoot = CreateDefaultSubobject<USceneComponent>(TEXT("FaceRoot"));
    FaceRoot->SetupAttachment(Root);
    FaceRoot->SetMobility(EComponentMobility::Movable);

    // Face Planes
    BrowsPlane  = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BrowsPlane"));
    EyesPlane   = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EyesPlane"));
    NosePlane  = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NosePlane"));
    MouthPlane  = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MouthPlane"));
    AccessoryPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AccessoryPlane"));
    SymbolPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SymbolPlane"));
    DetailPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DetailPlane"));

    BrowsPlane->SetupAttachment(FaceRoot);
    EyesPlane->SetupAttachment(FaceRoot);
    NosePlane->SetupAttachment(FaceRoot);
    MouthPlane->SetupAttachment(FaceRoot);
    AccessoryPlane->SetupAttachment(FaceRoot);
    SymbolPlane->SetupAttachment(FaceRoot);
    DetailPlane->SetupAttachment(FaceRoot);

    // Default meshes for viewport preview
    static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
    static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane.Plane"));

    if (SphereMesh.Succeeded())
    {
        BaseMesh->SetStaticMesh(SphereMesh.Object);
    }

    if (PlaneMesh.Succeeded())
    {
        BrowsPlane->SetStaticMesh(PlaneMesh.Object);
        EyesPlane->SetStaticMesh(PlaneMesh.Object);
        NosePlane->SetStaticMesh(PlaneMesh.Object);
        MouthPlane->SetStaticMesh(PlaneMesh.Object);
        AccessoryPlane->SetStaticMesh(PlaneMesh.Object);
        SymbolPlane->SetStaticMesh(PlaneMesh.Object);
        DetailPlane->SetStaticMesh(PlaneMesh.Object);
    }

    // Disable collisions for planes
    BrowsPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    EyesPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    NosePlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MouthPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    AccessoryPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SymbolPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    DetailPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when properties are changed in editor or actor is spawned
void AMaskActor::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (HasAnyFlags(RF_ClassDefaultObject)) return;

    ApplyBaseShape();

    if (bAutoLayout)
    {
        UpdateFaceOffsets();
    }

    // Delay material reapply
    bNeedsMaterialRefresh = true;

    // Reapply textures safely
    SetFacePart(EFacePartCategory::Brows, BrowsTexture);
    SetFacePart(EFacePartCategory::Eyes, EyesTexture);
    SetFacePart(EFacePartCategory::Nose, NoseTexture);
    SetFacePart(EFacePartCategory::Mouth, MouthTexture);
    SetFacePart(EFacePartCategory::Accessory, AccessoryTexture);
    SetFacePart(EFacePartCategory::Symbol, SymbolTexture);
    SetFacePart(EFacePartCategory::Detail, DetailTexture);

    ApplyHeadMaterial();
}

#if WITH_EDITOR
void AMaskActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (HasAnyFlags(RF_ClassDefaultObject)) return;

    ApplyBaseShape();

    if (bAutoLayout)
    {
        UpdateFaceOffsets();
    }

    ApplyHeadMaterial();
}

void AMaskActor::PostRegisterAllComponents()
{
    Super::PostRegisterAllComponents();

    if (HasAnyFlags(RF_ClassDefaultObject)) return;

    if (bNeedsMaterialRefresh)
    {
        bNeedsMaterialRefresh = false;

        SetFacePart(EFacePartCategory::Brows, BrowsTexture);
        SetFacePart(EFacePartCategory::Eyes, EyesTexture);
        SetFacePart(EFacePartCategory::Nose, NoseTexture);
        SetFacePart(EFacePartCategory::Mouth, MouthTexture);
        SetFacePart(EFacePartCategory::Accessory, AccessoryTexture);
        SetFacePart(EFacePartCategory::Symbol, SymbolTexture);
        SetFacePart(EFacePartCategory::Detail, DetailTexture);
    }

    ApplyHeadMaterial();
}

#endif

// Apply selected base shape
void AMaskActor::ApplyBaseShape()
{
    if (!BaseMesh) return;

    UStaticMesh* MeshToUse = nullptr;

    switch (BaseShape)
    {
        case EMaskBaseShape::Sphere:
            MeshToUse = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Sphere.Sphere"));
            break;

        case EMaskBaseShape::Cube:
            MeshToUse = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
            break;

        case EMaskBaseShape::Cylinder:
            MeshToUse = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
            break;

        case EMaskBaseShape::Capsule:
            MeshToUse = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Capsule.Capsule"));
            break;

        case EMaskBaseShape::Custom:
            MeshToUse = CustomBaseMesh;
            break;
    }

    if (MeshToUse)
    {
        BaseMesh->SetStaticMesh(MeshToUse);
    }
}

void AMaskActor::UpdateFaceOffsets()
{
    if (!BaseMesh || !FaceRoot) return;

    FVector Forward = GetActorForwardVector();

    // Front surface offset
    FVector Origin, Extent;
    BaseMesh->GetLocalBounds(Origin, Extent);
    float SphereRadius = Extent.X * BaseMesh->GetComponentScale().X;
    FVector BaseOffset = Forward * SphereRadius;

    // -----------------------------
    // Apply your working rotation to FaceRoot
    // -----------------------------
    FQuat YawQuat   = FRotationMatrix::MakeFromX(Forward).ToQuat();
    FQuat PitchQuat = FQuat(FVector::RightVector, FMath::DegreesToRadians(90.f));
    FQuat RollQuat  = FQuat(FVector::ForwardVector, FMath::DegreesToRadians(-90.f));

    FQuat FinalQuat = RollQuat * PitchQuat * YawQuat;

    // Rotate FaceRoot itself
    FaceRoot->SetRelativeRotation(FinalQuat);

    FaceRoot->SetRelativeLocation(FaceRootOffset);

    // -----------------------------
    // Planes are now identity relative to FaceRoot
    // -----------------------------
    FQuat PlaneQuat = FQuat::Identity;

    BrowsPlane->SetRelativeRotation(PlaneQuat);
    EyesPlane->SetRelativeRotation(PlaneQuat);
    NosePlane->SetRelativeRotation(PlaneQuat);
    MouthPlane->SetRelativeRotation(PlaneQuat);
    AccessoryPlane->SetRelativeRotation(PlaneQuat);
    SymbolPlane->SetRelativeRotation(PlaneQuat);
    DetailPlane->SetRelativeRotation(PlaneQuat);

    // -----------------------------
    // Position planes along FaceRoot local axes
    // -----------------------------
    FVector PlaneBasePos = Forward * ForwardSpacing; // small offset along sphere forward
    BrowsPlane->SetRelativeLocation(PlaneBasePos + FVector(0,0,BrowsHeight));
    EyesPlane->SetRelativeLocation(PlaneBasePos + FVector(0,0,EyesHeight));
    NosePlane->SetRelativeLocation(PlaneBasePos + FVector(0,0,NoseHeight));
    MouthPlane->SetRelativeLocation(PlaneBasePos + FVector(0,0,MouthHeight));
    AccessoryPlane->SetRelativeLocation(PlaneBasePos + FVector(0,0,AccessoryHeight));
    SymbolPlane->SetRelativeLocation(PlaneBasePos + FVector(0,0,SymbolHeight));
    DetailPlane->SetRelativeLocation(PlaneBasePos + FVector(0,0,DetailHeight));

    // ================= SCALE =================
    BrowsPlane->SetRelativeScale3D(FVector(GlobalFaceScale * BrowsScale));
    EyesPlane->SetRelativeScale3D(FVector(GlobalFaceScale * EyesScale));
    NosePlane->SetRelativeScale3D(FVector(GlobalFaceScale * NoseScale));
    MouthPlane->SetRelativeScale3D(FVector(GlobalFaceScale * MouthScale));
    AccessoryPlane->SetRelativeScale3D(FVector(GlobalFaceScale * AccessoryScale));
    SymbolPlane->SetRelativeScale3D(FVector(GlobalFaceScale * SymbolScale));
    DetailPlane->SetRelativeScale3D(FVector(GlobalFaceScale * DetailScale));

    // Refresh render state
    BrowsPlane->MarkRenderStateDirty();
    EyesPlane->MarkRenderStateDirty();
    NosePlane->MarkRenderStateDirty();
    MouthPlane->MarkRenderStateDirty();
    AccessoryPlane->MarkRenderStateDirty();
    SymbolPlane->MarkRenderStateDirty();
    DetailPlane->MarkRenderStateDirty();

    // Make faces write to custom depth
    BrowsPlane->SetRenderCustomDepth(true);
    EyesPlane->SetRenderCustomDepth(true);
    NosePlane->SetRenderCustomDepth(true);
    MouthPlane->SetRenderCustomDepth(true);
    AccessoryPlane->SetRenderCustomDepth(true);
    SymbolPlane->SetRenderCustomDepth(true);
    DetailPlane->SetRenderCustomDepth(true);

    // Give them a unique stencil value
    BrowsPlane->CustomDepthStencilValue = 1;
    EyesPlane->CustomDepthStencilValue = 1;
    NosePlane->CustomDepthStencilValue = 1;
    MouthPlane->CustomDepthStencilValue = 1;
    AccessoryPlane->CustomDepthStencilValue = 1;
    SymbolPlane->CustomDepthStencilValue = 1;
    DetailPlane->CustomDepthStencilValue = 1;

}


// Swap a face part texture
void AMaskActor::SetFacePart(EFacePartCategory Category, UTexture2D* FaceTexture)
{
    if (HasAnyFlags(RF_ClassDefaultObject)) return;
    
    if (!FaceTexture || !FaceMasterMaterial) return;

    UStaticMeshComponent* TargetPlane = nullptr;

    switch (Category)
    {
    case EFacePartCategory::Brows:     TargetPlane = BrowsPlane;     BrowsTexture = FaceTexture; break;
    case EFacePartCategory::Eyes:      TargetPlane = EyesPlane;      EyesTexture  = FaceTexture; break;
    case EFacePartCategory::Nose:      TargetPlane = NosePlane;      NoseTexture  = FaceTexture; break;
    case EFacePartCategory::Mouth:     TargetPlane = MouthPlane;     MouthTexture = FaceTexture; break;
    case EFacePartCategory::Accessory: TargetPlane = AccessoryPlane; AccessoryTexture = FaceTexture; break;
    case EFacePartCategory::Symbol:    TargetPlane = SymbolPlane;    SymbolTexture = FaceTexture; break;
    case EFacePartCategory::Detail:    TargetPlane = DetailPlane;    DetailTexture = FaceTexture; break;
    }

    if (!TargetPlane) return;

    // 🚨 Ensure material exists FIRST
    if (TargetPlane->GetMaterial(0) != FaceMasterMaterial)
    {
        TargetPlane->SetMaterial(0, FaceMasterMaterial);
    }

    // Now safe to create MID
    UMaterialInstanceDynamic* MID =
        Cast<UMaterialInstanceDynamic>(TargetPlane->GetMaterial(0));

    if (!MID)
    {
        MID = UMaterialInstanceDynamic::Create(FaceMasterMaterial, this);
        TargetPlane->SetMaterial(0, MID);
    }

    if (MID)
    {
        MID->SetTextureParameterValue(TEXT("FaceTexture"), FaceTexture);
    }
}

void AMaskActor::ApplyHeadMaterial()
{
    if (!BaseMesh || !FaceBaseMaterial) return;

    if (!DynamicHeadMaterial)
    {
        DynamicHeadMaterial =
            UMaterialInstanceDynamic::Create(FaceBaseMaterial, this);

        BaseMesh->SetMaterial(0, DynamicHeadMaterial);
    }

    DynamicHeadMaterial->SetVectorParameterValue(
        TEXT("FaceColor"),
        HeadColor
    );
}

