// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GAS_CharacterBase.h"
#include "GAS_CustomCharacter.generated.h"

class UHairStyleDataAsset;

UENUM(BlueprintType)
enum class EFacePartCategory : uint8
{
	Brows,
	Eyes,
	Nose,
	Mouth,
	Accessory,
	Symbol,
	Detail
};

UCLASS()
class THEOTLEYRUN_API AGAS_CustomCharacter : public AGAS_CharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGAS_CustomCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(
		FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	virtual void PostLoad() override;


public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USceneComponent* HeadRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* BaseMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* FacePlane;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Head")
    UMaterialInterface* FaceMasterMaterial;

    UPROPERTY(Transient)
    UMaterialInstanceDynamic* DynamicFaceMaterial;

    UPROPERTY(Transient)
    UMaterialInstanceDynamic* DynamicHeadMaterial;
    
	UPROPERTY(EditAnywhere, Category="Head")
    TArray<UHairStyleDataAsset*> AvailableHairStyles;
    
    UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* HairMesh;

    UPROPERTY(Transient)
    UMaterialInstanceDynamic* HairMID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Head")
    UTexture2D* BrowsTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Head")
    UTexture2D* EyesTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Head")
    UTexture2D* NoseTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Head")
    UTexture2D* MouthTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Head")
    UTexture2D* AccessoryTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Head")
    UTexture2D* SymbolTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Head")
    UTexture2D* DetailTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Head")
    float FaceScale = 1.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Head")
    FLinearColor HeadColor = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Head")
    FLinearColor HairColor = FLinearColor::Gray;

    UFUNCTION(BlueprintCallable)
    void UpdateFaceTransform();

    UFUNCTION(BlueprintCallable)
    void ApplyHeadMaterial();

    UFUNCTION(BlueprintCallable)
    void CreateFaceMaterial();

    UFUNCTION(BlueprintCallable)
    void RefreshAllFaceParts();

    UFUNCTION(BlueprintCallable)
    void SetFacePart(
        EFacePartCategory Category,
        UTexture2D* FaceTexture);
    
    UFUNCTION(BlueprintCallable)
    void SwapHair(const UHairStyleDataAsset* HairStyle);
    void SetHairTint(const FLinearColor& Tint) const;
};
