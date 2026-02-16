#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MaskActor.generated.h"

// Enum for selecting base shape
UENUM(BlueprintType)
enum class EMaskBaseShape : uint8
{
    Sphere      UMETA(DisplayName = "Sphere"),
    Cube        UMETA(DisplayName = "Cube"),
    Cylinder    UMETA(DisplayName = "Cylinder"),
    Capsule     UMETA(DisplayName = "Capsule"),
    Custom      UMETA(DisplayName = "Custom Mesh")
};

// Enum for face part categories
UENUM(BlueprintType)
enum class EFacePartCategory : uint8
{
    Brows   UMETA(DisplayName = "Brows"),
    Eyes    UMETA(DisplayName = "Eyes"),
    Nose    UMETA(DisplayName = "Nose"),
    Mouth   UMETA(DisplayName = "Mouth"),
    Accessory UMETA(DisplayName = "Accessory"),
    Symbol  UMETA(DisplayName = "Symbol"),
    Detail  UMETA(DisplayName = "Detail")
};

UCLASS()
class THEOTLEYRUN_API AMaskActor : public AActor
{
    GENERATED_BODY()
private:
    bool bNeedsMaterialRefresh = false;

public:	
    // Sets default values
    AMaskActor();

    // Called when properties change in editor or actor is spawned
    virtual void OnConstruction(const FTransform& Transform) override;
    virtual void Tick(float DeltaTime) override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostRegisterAllComponents() override;
#endif

    UPROPERTY(EditDefaultsOnly, Category="00 Mask")
    UMaterialInterface* FaceMasterMaterial;

    UPROPERTY(EditAnywhere, Category="00 Mask|Faces")
    UTexture2D* BrowsTexture;

    UPROPERTY(EditAnywhere, Category="00 Mask|Faces")
    UTexture2D* EyesTexture;

    UPROPERTY(EditAnywhere, Category="00 Mask|Faces")
    UTexture2D* NoseTexture;

    UPROPERTY(EditAnywhere, Category="00 Mask|Faces")
    UTexture2D* MouthTexture;

    UPROPERTY(EditAnywhere, Category="00 Mask|Faces")
    UTexture2D* AccessoryTexture;

    UPROPERTY(EditAnywhere, Category="00 Mask|Faces")
    UTexture2D* SymbolTexture;

    UPROPERTY(EditAnywhere, Category="00 Mask|Faces")
    UTexture2D* DetailTexture;

    
    // Swap a face part texture
    UFUNCTION(BlueprintCallable, Category="00 Mask")
    void SetFacePart(EFacePartCategory Category, UTexture2D* FaceTexture);
    void ApplyHeadMaterial();

    // Apply base shape mesh
    UFUNCTION(BlueprintCallable, Category="00 Mask")
    void ApplyBaseShape();

    // Update face plane offsets based on mesh size
    UFUNCTION(BlueprintCallable, Category="00 Mask")
    void UpdateFaceOffsets();

    UPROPERTY(EditAnywhere, Category="00 Mask")
    bool bAutoLayout = true;

    // --- Base shape selection ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="00 Mask")
    EMaskBaseShape BaseShape = EMaskBaseShape::Sphere;

    // Optional custom mesh for BaseShape = Custom
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="00 Mask")
    UStaticMesh* CustomBaseMesh = nullptr;

    // --- Components ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="00 Mask")
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="00 Mask")
    UStaticMeshComponent* BaseMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="00 Mask")
    USceneComponent* FaceRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="00 Mask")
    UStaticMeshComponent* BrowsPlane;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="00 Mask")
    UStaticMeshComponent* EyesPlane;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="00 Mask")
    UStaticMeshComponent* NosePlane;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="00 Mask")
    UStaticMeshComponent* MouthPlane;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="00 Mask")
    UStaticMeshComponent* AccessoryPlane;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="00 Mask")
    UStaticMeshComponent* SymbolPlane;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="00 Mask")
    UStaticMeshComponent* DetailPlane;

    UPROPERTY(EditAnywhere, Category="00 Mask")
    float BrowsHeight = 0.f;
    
    UPROPERTY(EditAnywhere, Category="00 Mask")
    float EyesHeight = 0.f;

    UPROPERTY(EditAnywhere, Category="00 Mask")
    float NoseHeight;

    UPROPERTY(EditAnywhere, Category="00 Mask")
    float MouthHeight = 0.f;

    UPROPERTY(EditAnywhere, Category="00 Mask")
    float AccessoryHeight = 0.f;

    UPROPERTY(EditAnywhere, Category="00 Mask")
    float SymbolHeight = 0.f;

    UPROPERTY(EditAnywhere, Category="00 Mask")
    float DetailHeight = 0.f;

    UPROPERTY(EditAnywhere, Category="00 Mask")
    float ForwardSpacing = 0.05f;

    UPROPERTY(EditAnywhere, Category="00 Mask")
    float GlobalFaceScale = 1.f;
    
    UPROPERTY(EditAnywhere, Category="00 Mask")
    float EyesScale = 1.f;
    
    UPROPERTY(EditAnywhere, Category="00 Mask")
    float NoseScale = 1.f;
    
    UPROPERTY(EditAnywhere, Category="00 Mask")
    float MouthScale = 1.f;
    
    UPROPERTY(EditAnywhere, Category="00 Mask")
    float BrowsScale = 1.f;

    UPROPERTY(EditAnywhere, Category="00 Mask")
    float AccessoryScale = 1.f;
    
    UPROPERTY(EditAnywhere, Category="00 Mask")
    float SymbolScale = 1.f;
    
    UPROPERTY(EditAnywhere, Category="00 Mask")
    float DetailScale = 1.f;
    
    // Editor tweak for manually moving the face
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="00 Mask|Editor")
    FVector FaceRootOffset = FVector::ZeroVector;

protected:

    // Base face material (assigned in editor)
    UPROPERTY(EditDefaultsOnly, Category = "00 Mask|Material")
    UMaterialInterface* FaceBaseMaterial = nullptr;

    // Runtime dynamic material
    UPROPERTY(Transient)
    UMaterialInstanceDynamic* DynamicHeadMaterial = nullptr;

    // Skin / head color
    UPROPERTY(EditAnywhere, Category="00 Mask|Color")
    FLinearColor HeadColor = FLinearColor::White;
};
