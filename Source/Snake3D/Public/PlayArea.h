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
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Grid")
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
	UFUNCTION()
	void OnFoodDestroyed(AActor* Food);
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
private:
	void SpawnTiles() const;
	FVector GetRandomFloorLocation() const;
	void SpawnFood() const;
};
