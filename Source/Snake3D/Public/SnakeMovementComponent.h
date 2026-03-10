// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SnakeMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class SNAKE3D_API USnakeMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	
public:
	USnakeMovementComponent();
	
	virtual void TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	// Takes a readonly reference from the owning pawn and stores it as a const pointer
	void BindMovementSpeed(const float* InSpeed);
	
private:
	const float* Speed = nullptr;
	void MoveTick(const float DeltaTime);
};
