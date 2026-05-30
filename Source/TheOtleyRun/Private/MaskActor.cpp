#include "MaskActor.h"

#include "Components/StaticMeshComponent.h"
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

void AMaskActor::PostLoad()
{
    Super::PostLoad();

    CreateFaceMaterial();
    RefreshAllFaceParts();
    ApplyHeadMaterial();
    UpdateFaceTransform();
}

#endif

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
    {
        return;
    }

    UMaterialInterface* CurrentMat = FacePlane->GetMaterial(0);

    if (!CurrentMat || CurrentMat->GetMaterial() != FaceMasterMaterial)
    {
        DynamicFaceMaterial =
            UMaterialInstanceDynamic::Create(
                FaceMasterMaterial,
                this);

        FacePlane->SetMaterial(0, DynamicFaceMaterial);
    }
    else
    {
        DynamicFaceMaterial =
            Cast<UMaterialInstanceDynamic>(CurrentMat);

        if (!DynamicFaceMaterial)
        {
            DynamicFaceMaterial =
                UMaterialInstanceDynamic::Create(
                    FaceMasterMaterial,
                    this);

            FacePlane->SetMaterial(0, DynamicFaceMaterial);
        }
    }
}

void AMaskActor::RefreshAllFaceParts()
{
    if (!DynamicFaceMaterial)
    {
        return;
    }

    DynamicFaceMaterial->SetTextureParameterValue(TEXT("BrowsTex"),BrowsTexture);
    DynamicFaceMaterial->SetTextureParameterValue(TEXT("EyesTex"),EyesTexture);
    DynamicFaceMaterial->SetTextureParameterValue(TEXT("NoseTex"),NoseTexture);
    DynamicFaceMaterial->SetTextureParameterValue(TEXT("MouthTex"),MouthTexture);
    DynamicFaceMaterial->SetTextureParameterValue(TEXT("AccessoryTex"),AccessoryTexture);
    DynamicFaceMaterial->SetTextureParameterValue(TEXT("SymbolTex"),SymbolTexture);
    DynamicFaceMaterial->SetTextureParameterValue(TEXT("DetailTex"),DetailTexture);
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
    if (!BaseMesh)
    {
        return;
    }

    if (!DynamicHeadMaterial)
    {
        DynamicHeadMaterial = BaseMesh->CreateAndSetMaterialInstanceDynamic(0);

        BaseMesh->SetMaterial(
            0,
            DynamicHeadMaterial);
    }

    DynamicHeadMaterial->SetVectorParameterValue(
        TEXT("FaceColor"),
        HeadColor);
}