// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Food.generated.h"



UCLASS()
class SNAKE3D_API AFood : public AActor
{
	GENERATED_BODY()
	
	// --- Properties ---
protected:
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	
	// --- Methods ---
public:	
	// Sets default values for this actor's properties
	AFood();
};
