#include "MaskActor.h"

#include "HairStyleDataAsset.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "UObject/ConstructorHelpers.h"

AMaskActor::AMaskActor()
{
    PrimaryActorTick.bCanEverTick = false;
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    SetRootComponent(Root);
    BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
    BaseMesh->SetupAttachment(Root);
    FacePlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FacePlane"));
    FacePlane->SetupAttachment(Root);
    FacePlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    static ConstructorHelpers::FObjectFinder<UStaticMesh>PlaneMesh(TEXT("/Engine/BasicShapes/Plane.Plane"));
    if (PlaneMesh.Succeeded())
    {
        FacePlane->SetStaticMesh(PlaneMesh.Object);
    }

    HairMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HairMesh"));
    HairMesh->SetupAttachment(Root);
    HairMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    HairMesh->SetRelativeLocation(FVector::ZeroVector);
    HairMesh->SetRelativeRotation(FRotator::ZeroRotator);
    HairMesh->SetRelativeScale3D(FVector(1.f));

    SetActorEnableCollision(false);
}

void AMaskActor::BeginPlay()
{
    Super::BeginPlay();
    CreateFaceMaterial();
    RefreshAllFaceParts();
    ApplyHeadMaterial();
}

void AMaskActor::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    DynamicFaceMaterial = nullptr;
    DynamicHeadMaterial = nullptr;

    UpdateFaceTransform();
    CreateFaceMaterial();
    RefreshAllFaceParts();
    ApplyHeadMaterial();
}

#if WITH_EDITOR

void AMaskActor::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
    UpdateFaceTransform();
    CreateFaceMaterial();
    RefreshAllFaceParts();
    ApplyHeadMaterial();
}
#endif

void AMaskActor::PostLoad()
{
    Super::PostLoad();

    CreateFaceMaterial();
    RefreshAllFaceParts();
    ApplyHeadMaterial();
    UpdateFaceTransform();
}
void AMaskActor::UpdateFaceTransform()
{
    if (!FacePlane)
    {
        return;
    }

    FacePlane->SetRelativeScale3D(
        FVector(FaceScale));
}

void AMaskActor::CreateFaceMaterial()
{
    if (!FaceMasterMaterial || !FacePlane)
        return;

    bool bNeedsRecreate = true;

    if (DynamicFaceMaterial)
    {
        // Check the DMI is still valid and not pending kill
        if (IsValid(DynamicFaceMaterial) && 
            DynamicFaceMaterial->GetMaterial() == FaceMasterMaterial)
        {
            bNeedsRecreate = false;
        }
    }

    if (bNeedsRecreate)
    {
        DynamicFaceMaterial = UMaterialInstanceDynamic::Create(
            FaceMasterMaterial, this);
    }

    FacePlane->SetMaterial(0, DynamicFaceMaterial);
}

void AMaskActor::RefreshAllFaceParts()
{
    if (!DynamicFaceMaterial) return;

    auto SafeSet = [&](const FName& ParamName, UTexture2D* Tex)
    {
        if (Tex)
            DynamicFaceMaterial->SetTextureParameterValue(ParamName, Tex);
    };

    SafeSet(TEXT("BrowsTex"),      BrowsTexture);
    SafeSet(TEXT("EyesTex"),       EyesTexture);
    SafeSet(TEXT("NoseTex"),       NoseTexture);
    SafeSet(TEXT("MouthTex"),      MouthTexture);
    SafeSet(TEXT("AccessoryTex"),  AccessoryTexture);
    SafeSet(TEXT("SymbolTex"),     SymbolTexture);
    SafeSet(TEXT("DetailTex"),     DetailTexture);
}

void AMaskActor::SetFacePart(EFacePartCategory Category, UTexture2D* FaceTexture)
{
    if (!DynamicFaceMaterial || !FaceTexture)
    {
        return;
    }

    switch (Category)
    {
        case EFacePartCategory::Brows:
        {
            BrowsTexture = FaceTexture;

            DynamicFaceMaterial->SetTextureParameterValue(
                TEXT("BrowsTex"),
                FaceTexture);

            break;
        }

        case EFacePartCategory::Eyes:
        {
            EyesTexture = FaceTexture;

            DynamicFaceMaterial->SetTextureParameterValue(
                TEXT("EyesTex"),
                FaceTexture);

            break;
        }

        case EFacePartCategory::Nose:
        {
            NoseTexture = FaceTexture;

            DynamicFaceMaterial->SetTextureParameterValue(
                TEXT("NoseTex"),
                FaceTexture);

            break;
        }

        case EFacePartCategory::Mouth:
        {
            MouthTexture = FaceTexture;

            DynamicFaceMaterial->SetTextureParameterValue(
                TEXT("MouthTex"),
                FaceTexture);

            break;
        }

        case EFacePartCategory::Accessory:
        {
            AccessoryTexture = FaceTexture;

            DynamicFaceMaterial->SetTextureParameterValue(
                TEXT("AccessoryTex"),
                FaceTexture);

            break;
        }

        case EFacePartCategory::Symbol:
        {
            SymbolTexture = FaceTexture;

            DynamicFaceMaterial->SetTextureParameterValue(
                TEXT("SymbolTex"),
                FaceTexture);

            break;
        }

        case EFacePartCategory::Detail:
        {
            DetailTexture = FaceTexture;

            DynamicFaceMaterial->SetTextureParameterValue(
                TEXT("DetailTex"),
                FaceTexture);

            break;
        }
    }
}

void AMaskActor::ApplyHeadMaterial()
{
    if (!BaseMesh) return;

    UStaticMesh* Mesh = BaseMesh->GetStaticMesh();
    if (!Mesh) return;

    UMaterialInterface* SourceMaterial = Mesh->GetMaterial(0);
    if (!SourceMaterial) return;

    DynamicHeadMaterial = UMaterialInstanceDynamic::Create(
        SourceMaterial, this);

    BaseMesh->SetMaterial(0, DynamicHeadMaterial);

    DynamicHeadMaterial->SetVectorParameterValue(
        TEXT("FaceColor"), HeadColor);

    SwapHair(AvailableHairStyles[0]);
    SetHairTint(FLinearColor(0.2f, 0.8f, 1.0f)); // icy blue hair
}

void AMaskActor::SwapHair(const UHairStyleDataAsset* HairStyle)
{
    if (!HairMesh || !HairStyle || !HairStyle->Mesh)
        return;

    HairMesh->SetStaticMesh(HairStyle->Mesh);
    HairMesh->SetRelativeLocation(HairStyle->RelativeLocation);
    HairMesh->SetRelativeRotation(HairStyle->RelativeRotation);
    HairMesh->SetRelativeScale3D(HairStyle->RelativeScale);

    if (HairStyle->HairMaterial)
    {
        UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(
            HairStyle->HairMaterial,
            this
        );

        HairMesh->SetMaterial(0, MID);

        // store it if you want later tint updates
        HairMID = MID;
    }
}

void AMaskActor::SetHairTint(const FLinearColor& Tint) const
{
    if (HairMID)
    {
        HairMID->SetVectorParameterValue(TEXT("HairTint"), Tint);
    }
}
