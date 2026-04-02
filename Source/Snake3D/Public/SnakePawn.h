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
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> HeadMesh;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USnakeMovementComponent> Movement;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> CameraSpring;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UCameraComponent> Camera;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Body")
	int NrOfBodyCells;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Body")
	TSubclassOf<AActor> BodyCellActorClass;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Body")
	TArray<TObjectPtr<AActor>> BodyCellActors;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Body")
	TArray<TObjectPtr<UChildActorComponent>> ChildActorComponents;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Body")
	float BodyCellOffset;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Movement")
	float MovementSpeed;

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
	bool AddBodyCell();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	void MoveBodyCells(float DeltaTime);
};
