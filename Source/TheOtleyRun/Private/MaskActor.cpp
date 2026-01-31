#include "MaskActor.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
AMaskActor::AMaskActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // Root
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);

    // Base Mesh
    BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
    BaseMesh->SetupAttachment(Root);

    // Face Root (holds all planes)
    FaceRoot = CreateDefaultSubobject<USceneComponent>(TEXT("FaceRoot"));
    FaceRoot->SetupAttachment(BaseMesh);

    // Face Planes
    EyesPlane   = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("EyesPlane"));
    MouthPlane  = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MouthPlane"));
    BrowsPlane  = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BrowsPlane"));
    SymbolPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SymbolPlane"));
    DetailPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DetailPlane"));

    EyesPlane->SetupAttachment(FaceRoot);
    MouthPlane->SetupAttachment(FaceRoot);
    BrowsPlane->SetupAttachment(FaceRoot);
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
        EyesPlane->SetStaticMesh(PlaneMesh.Object);
        MouthPlane->SetStaticMesh(PlaneMesh.Object);
        BrowsPlane->SetStaticMesh(PlaneMesh.Object);
        SymbolPlane->SetStaticMesh(PlaneMesh.Object);
        DetailPlane->SetStaticMesh(PlaneMesh.Object);
    }

    // Disable collisions for planes
    EyesPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    MouthPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    BrowsPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    SymbolPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    DetailPlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when properties are changed in editor or actor is spawned
void AMaskActor::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    ApplyBaseShape();
    UpdateFaceOffsets();
}

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

    FVector Forward = BaseMesh->GetForwardVector();

    // Front surface offset
    FVector Origin, Extent;
    BaseMesh->GetLocalBounds(Origin, Extent);
    FVector BaseOffset = Forward * Extent.X;

    float ForwardSpacing = 0.05f;

    // --- ROTATION SETUP ---

    // 1. Yaw: align plane to mesh forward
    FQuat YawQuat = FRotationMatrix::MakeFromX(Forward).ToQuat();

    // 2. Pitch: stand the plane upright (rotate around Y axis)
    FQuat PitchQuat = FQuat(FVector::RightVector, FMath::DegreesToRadians(90.f));

    // 3. Roll: rotate 90° around plane’s forward axis
    FQuat RollQuat = FQuat(FVector::ForwardVector, FMath::DegreesToRadians(-90.f));

    // Order matters: Roll * Pitch * Yaw
    FQuat FinalQuat = RollQuat * PitchQuat * YawQuat;

    // Apply rotation
    EyesPlane->SetRelativeRotation(FinalQuat);
    MouthPlane->SetRelativeRotation(FinalQuat);
    BrowsPlane->SetRelativeRotation(FinalQuat);
    SymbolPlane->SetRelativeRotation(FinalQuat);
    DetailPlane->SetRelativeRotation(FinalQuat);

    // Position (all share same base position, vertical offsets handled later)
    FVector PlaneBasePos = BaseOffset + Forward * ForwardSpacing;

    EyesPlane->SetRelativeLocation(PlaneBasePos);
    MouthPlane->SetRelativeLocation(PlaneBasePos);
    BrowsPlane->SetRelativeLocation(PlaneBasePos);
    SymbolPlane->SetRelativeLocation(PlaneBasePos);
    DetailPlane->SetRelativeLocation(PlaneBasePos);
}


// Swap a face part texture
void AMaskActor::SetFacePart(EFacePartCategory Category, UTexture2D* FaceTexture)
{
    UStaticMeshComponent* TargetPlane = nullptr;

    switch (Category)
    {
        case EFacePartCategory::Eyes:    TargetPlane = EyesPlane;   break;
        case EFacePartCategory::Mouth:   TargetPlane = MouthPlane;  break;
        case EFacePartCategory::Brows:   TargetPlane = BrowsPlane;  break;
        case EFacePartCategory::Symbol:  TargetPlane = SymbolPlane; break;
        case EFacePartCategory::Detail:  TargetPlane = DetailPlane; break;
    }

    if (!TargetPlane || !FaceTexture) return;

    UMaterialInstanceDynamic* MID = TargetPlane->CreateDynamicMaterialInstance(0);
    if (MID)
    {
        MID->SetTextureParameterValue(FName("FaceTexture"), FaceTexture);
    }
}
