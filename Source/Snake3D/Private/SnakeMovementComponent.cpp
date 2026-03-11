// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakeMovementComponent.h"
#include "SnakePawn.h"

USnakeMovementComponent::USnakeMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USnakeMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	
	SnakePawn = Cast<ASnakePawn>(GetOwner());
	if (!IsValid(SnakePawn))
	{
		UE_LOG(LogTemp, Error, TEXT("SnakeMovementComponent::BeginPlay - Owning actor casted to SnakePawn is invalid"));
	}
}

void USnakeMovementComponent::TickComponent(const float DeltaTime, const ELevelTick TickType,
                                            FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	MoveTick(DeltaTime);
}

void USnakeMovementComponent::MoveTick(const float DeltaTime)
{
	if (!IsValid(UpdatedComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("SnakeMovementComponent::MoveTick - UpdatedComponent is invalid"));
		return;
	}
	
	const FVector InputVector = ConsumeInputVector();
	if (!InputVector.IsNearlyZero())
	{
		const FRotator DeltaRotation(0.0, InputVector.Z * 90.0, 0.0);
		UpdatedComponent->AddRelativeRotation(DeltaRotation);
	}
	
	if (!IsValid(SnakePawn))
	{
		UE_LOG(LogTemp, Error, TEXT("SnakeMovementComponent::MoveTick - SnakePawn is invalid"));
		return;
	}
	
	const float Speed = SnakePawn->GetMovementSpeed();
	const FVector3d Move = FVector3d::ForwardVector * Speed * DeltaTime;
	UpdatedComponent->AddLocalOffset(Move, true);
}
