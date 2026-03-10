// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeMovementComponent.h"

USnakeMovementComponent::USnakeMovementComponent()
{
	UE_LOG(LogTemp, Log, TEXT("SnakeMovementComponent constructor called"));
	PrimaryComponentTick.bCanEverTick = true;
}

void USnakeMovementComponent::TickComponent(const float DeltaTime, const ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	MoveTick(DeltaTime);
}

void USnakeMovementComponent::BindMovementSpeed(const float* InSpeed)
{
	Speed = InSpeed;
}

void USnakeMovementComponent::MoveTick(const float DeltaTime)
{
	if (Speed == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("SnakeMovementComponent::MoveTick - Speed pointer is null. Cannot move. Did you forget to call BindMovementSpeed?"));
		return;
	}
	
	const FVector InputVector = this->ConsumeInputVector();
	if (!InputVector.IsNearlyZero())
	{
		UE_LOG(LogTemp, Log, TEXT("InputVector: %s"), *InputVector.ToString());
		
		const FRotator DeltaRotation(0.0, InputVector.Z * 90.0, 0.0);
		UE_LOG(LogTemp, Log, TEXT("Changing rotation by: %s"), *DeltaRotation.ToString());
		this->UpdatedComponent->AddRelativeRotation(DeltaRotation);
	}
	
	const FVector3d Move = FVector3d::ForwardVector * (*Speed) * DeltaTime;
	this->UpdatedComponent->AddLocalOffset(Move, true);
}
