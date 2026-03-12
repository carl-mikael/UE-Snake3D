// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SnakeBodyCell.generated.h"

class UStaticMeshComponent;

UCLASS()
class SNAKE3D_API ASnakeBodyCell : public AActor
{
	GENERATED_BODY()

	// --- Properties ---
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	// --- Methods ---
public:
	// Sets default values for this actor's properties
	ASnakeBodyCell();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
