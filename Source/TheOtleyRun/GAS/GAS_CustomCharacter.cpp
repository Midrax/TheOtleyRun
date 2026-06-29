// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS_CustomCharacter.h"
#include "HairStyleDataAsset.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/Texture2D.h"
#include "UObject/ConstructorHelpers.h"

AGAS_CustomCharacter::AGAS_CustomCharacter()
{
	HeadRoot = CreateDefaultSubobject<USceneComponent>(TEXT("HeadRoot"));
	HeadRoot->SetupAttachment(GetMesh(), TEXT("head")); // replace "head" with your actual socket name

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadMesh"));
	BaseMesh->SetupAttachment(HeadRoot);
	BaseMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BaseMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	BaseMesh->SetGenerateOverlapEvents(false);

	FacePlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FacePlane"));
	FacePlane->SetupAttachment(HeadRoot); // still rides along with the head mesh
	FacePlane->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FacePlane->SetCollisionResponseToAllChannels(ECR_Ignore);
	FacePlane->SetGenerateOverlapEvents(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> PlaneMesh(TEXT("/Engine/BasicShapes/Plane.Plane"));
	if (PlaneMesh.Succeeded())
	{
		FacePlane->SetStaticMesh(PlaneMesh.Object);
	}

	HairMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HairMesh"));
	HairMesh->SetupAttachment(HeadRoot);
	HairMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	HairMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	HairMesh->SetGenerateOverlapEvents(false);
	HairMesh->SetRelativeLocation(FVector::ZeroVector);
	HairMesh->SetRelativeRotation(FRotator::ZeroRotator);
	HairMesh->SetRelativeScale3D(FVector(1.f));
	
	HeadRoot->SetRelativeRotation(FRotator(0.f, 90.f, 90.f));
}

// Called when the game starts or when spawned
void AGAS_CustomCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	UpdateFaceTransform();
	CreateFaceMaterial();
	RefreshAllFaceParts();
	ApplyHeadMaterial();
}

void AGAS_CustomCharacter::OnConstruction(const FTransform& Transform)
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

void AGAS_CustomCharacter::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	UpdateFaceTransform();
	CreateFaceMaterial();
	RefreshAllFaceParts();
	ApplyHeadMaterial();
}
#endif

void AGAS_CustomCharacter::PostLoad()
{
	Super::PostLoad();

	UpdateFaceTransform();
	CreateFaceMaterial();
	RefreshAllFaceParts();
	ApplyHeadMaterial();
}

void AGAS_CustomCharacter::UpdateFaceTransform()
{
	if (!FacePlane)
	{
		return;
	}

	FacePlane->SetRelativeScale3D(
		FVector(FaceScale));
}

void AGAS_CustomCharacter::CreateFaceMaterial()
{
	if (!FaceMasterMaterial || !FacePlane)
		return;

	bool bNeedsRecreate = true;

	if (DynamicFaceMaterial)
	{
		// Check the DMI is still valid and not pending kill
		if (IsValid(DynamicFaceMaterial) && DynamicFaceMaterial->GetMaterial() == FaceMasterMaterial->GetMaterial())
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

void AGAS_CustomCharacter::RefreshAllFaceParts()
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

void AGAS_CustomCharacter::SetFacePart(EFacePartCategory Category, UTexture2D* FaceTexture)
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

void AGAS_CustomCharacter::ApplyHeadMaterial()
{
    if (!BaseMesh) return;

    UStaticMesh* HeadMesh = BaseMesh->GetStaticMesh();
    if (!HeadMesh) return;

    UMaterialInterface* SourceMaterial = HeadMesh->GetMaterial(0);
    if (!SourceMaterial) return;

    DynamicHeadMaterial = UMaterialInstanceDynamic::Create(
        SourceMaterial, this);

    BaseMesh->SetMaterial(0, DynamicHeadMaterial);

    DynamicHeadMaterial->SetVectorParameterValue(
        TEXT("FaceColor"), HeadColor);

    SwapHair(AvailableHairStyles[0]);
    SetHairTint(FLinearColor(HairColor));
}

void AGAS_CustomCharacter::SwapHair(const UHairStyleDataAsset* HairStyle)
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

void AGAS_CustomCharacter::SetHairTint(const FLinearColor& Tint) const
{
    if (HairMID)
    {
        HairMID->SetVectorParameterValue(TEXT("HairTint"), Tint);
    }
}

// Called every frame
void AGAS_CustomCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AGAS_CustomCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

