#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MaskActor.generated.h"

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
class THEOTLEYRUN_API AMaskActor : public AActor
{
    GENERATED_BODY()

public:

    AMaskActor();

protected:

    virtual void OnConstruction(const FTransform& Transform) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(
        FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostLoad() override;
#endif

public:

    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* BaseMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* FacePlane;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mask")
    UStaticMesh* CustomBaseMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Materials")
    UMaterialInterface* FaceMasterMaterial;

    UPROPERTY()
    UMaterialInstanceDynamic* DynamicFaceMaterial;

    UPROPERTY()
    UMaterialInstanceDynamic* DynamicHeadMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Face")
    UTexture2D* BrowsTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Face")
    UTexture2D* EyesTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Face")
    UTexture2D* NoseTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Face")
    UTexture2D* MouthTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Face")
    UTexture2D* AccessoryTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Face")
    UTexture2D* SymbolTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Face")
    UTexture2D* DetailTexture;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Face")
    float FaceScale = 1.f;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Head")
    FLinearColor HeadColor = FLinearColor::White;

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
};