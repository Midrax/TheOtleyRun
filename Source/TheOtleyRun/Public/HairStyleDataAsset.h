#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "HairStyleDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class THEOTLEYRUN_API UHairStyleDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere)
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UMaterialInterface* HairMaterial;

	UPROPERTY(EditAnywhere)
	FVector RelativeLocation;

	UPROPERTY(EditAnywhere)
	FRotator RelativeRotation;

	UPROPERTY(EditAnywhere)
	FVector RelativeScale;
};
