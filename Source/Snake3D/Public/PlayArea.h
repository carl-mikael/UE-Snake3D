// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PlayArea.generated.h"

UCLASS()
class SNAKE3D_API APlayArea : public AActor
{
	GENERATED_BODY()
	
	// --- Properties ---
protected:
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TObjectPtr<USceneComponent> SceneComponent;
	
	UPROPERTY(ReplicatedUsing=OnRep_GridSize)
	int GridSize;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Grid")
	int TileSize;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Floor")
	TObjectPtr<UInstancedStaticMeshComponent> FloorMeshInstances;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Floor")
	float TileScale;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Floor")
	float TileZOffset;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Wall")
	TObjectPtr<UInstancedStaticMeshComponent> WallMeshInstances;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Wall")
	float WallScale;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Food")
	TObjectPtr<UChildActorComponent> FoodChildActorComponent;
	
	// --- Methods ---
public:	
	// Sets default values for this actor's properties
	APlayArea();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void OnGameStageChanged(int NewGameStage);
	
	UFUNCTION()
	void OnRep_GridSize();
	
	UFUNCTION()
	void OnFoodDestroyed(AActor* Food);
	
	void RegenMap();

private:
	void SpawnTiles() const;
	FVector GetRandomFloorLocation() const;
	void SpawnFood() const;
};
