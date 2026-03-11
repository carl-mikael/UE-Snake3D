// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SnakeMovementComponent.generated.h"

class ASnakePawn;

UCLASS()
class SNAKE3D_API USnakeMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	
	// --- Properties ---
private:
	UPROPERTY()
	TObjectPtr<ASnakePawn> SnakePawn;
	void MoveTick(const float DeltaTime);
	
	// --- Methods ---
public:
	USnakeMovementComponent();
	virtual void BeginPlay() override;
	
protected:
	virtual void TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
