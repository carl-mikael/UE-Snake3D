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
	
	UPROPERTY(BlueprintReadOnly, Category = "Head")
	FName HeadMeshAssetPath;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Head")
	TObjectPtr<UStaticMeshComponent> HeadMesh;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Components")
	TObjectPtr<USnakeMovementComponent> Movement;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
	TObjectPtr<USpringArmComponent> CameraSpring;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Camera")
	TObjectPtr<UCameraComponent> Camera;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Body")
	FName BodyMeshAssetPath;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "Body")
	TArray<TObjectPtr<UStaticMeshComponent>> BodyCells;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Movement")
	float MovementSpeed;
	
	UPROPERTY()
	TArray<FTransform> AnchorPoints;

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
	
private:
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
