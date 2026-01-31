#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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
    Eyes    UMETA(DisplayName = "Eyes"),
    Mouth   UMETA(DisplayName = "Mouth"),
    Brows   UMETA(DisplayName = "Brows"),
    Symbol  UMETA(DisplayName = "Symbol"),
    Detail  UMETA(DisplayName = "Detail")
};

UCLASS()
class THEOTLEYRUN_API AMaskActor : public AActor
{
    GENERATED_BODY()

public:	
    // Sets default values
    AMaskActor();

    // Called when properties change in editor or actor is spawned
    virtual void OnConstruction(const FTransform& Transform) override;

    // Swap a face part texture
    UFUNCTION(BlueprintCallable, Category="Mask")
    void SetFacePart(EFacePartCategory Category, UTexture2D* FaceTexture);

    // Apply base shape mesh
    UFUNCTION(BlueprintCallable, Category="Mask")
    void ApplyBaseShape();

    // Update face plane offsets based on mesh size
    UFUNCTION(BlueprintCallable, Category="Mask")
    void UpdateFaceOffsets();

public:

    // --- Base shape selection ---
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mask")
    EMaskBaseShape BaseShape = EMaskBaseShape::Sphere;

    // Optional custom mesh for BaseShape = Custom
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Mask")
    UStaticMesh* CustomBaseMesh = nullptr;

    // --- Components ---
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mask")
    USceneComponent* Root;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mask")
    UStaticMeshComponent* BaseMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mask")
    USceneComponent* FaceRoot;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mask")
    UStaticMeshComponent* EyesPlane;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mask")
    UStaticMeshComponent* MouthPlane;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mask")
    UStaticMeshComponent* BrowsPlane;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mask")
    UStaticMeshComponent* SymbolPlane;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mask")
    UStaticMeshComponent* DetailPlane;
};
