// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SnakePawn.generated.h"

class USpringArmComponent;
class UCameraComponent;
class USnakeMovementComponent;
class USceneComponent;
class UStaticMeshComponent;

UCLASS()
class SNAKE3D_API ASnakePawn : public APawn
{
	GENERATED_BODY()

	// --- Properties ---
protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	TObjectPtr<USceneComponent> DummySceneComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	TObjectPtr<USnakeMovementComponent> MovementComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraSpringComponent;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> CameraComponent;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	float MovementSpeed = 200.0f;

	// --- Methods ---
public:
	ASnakePawn();
	virtual void Tick(const float DeltaTime) override;
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual UPawnMovementComponent* GetMovementComponent() const override;
	float GetMovementSpeed() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
