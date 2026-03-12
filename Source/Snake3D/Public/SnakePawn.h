// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SnakePawn.generated.h"

class UClass;
class UChildActorComponent;
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
	TObjectPtr<USceneComponent> DummyRoot;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "ChildComponent", meta=(OnlyPlaceable, AllowPrivateAccess="true"))
	TSubclassOf<AActor> ChildActorClass;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "ChildComponent")
	TArray<TObjectPtr<UChildActorComponent>> ChildActors;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> HeadMesh;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	TObjectPtr<USnakeMovementComponent> Movement;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraSpring;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;
	
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
	virtual void OnConstruction(const FTransform& Transform) override;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
};
