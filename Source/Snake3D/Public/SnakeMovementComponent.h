// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SnakeMovementComponent.generated.h"

UCLASS()
class SNAKE3D_API USnakeMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()
	
	// --- Properties ---
private:
	const float* Speed = nullptr;
	void MoveTick(const float DeltaTime);
	
	// --- Methods ---
public:
	USnakeMovementComponent();
	// Stores a reference to the InSpeed, used for moving the snake.
	void BindMovementSpeed(const float* InSpeed);
	
protected:
	virtual void TickComponent(const float DeltaTime, const ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
