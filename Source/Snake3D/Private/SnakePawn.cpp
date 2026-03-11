// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakePawn.h"

#include "SnakeMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ASnakePawn::ASnakePawn()
{
	PrimaryActorTick.bCanEverTick = true;

	DummySceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	SetRootComponent(DummySceneComponent);
	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	
	if (!StaticMesh.IsNull())
	{
		if (IsValid(UStaticMesh* LoadedStaticMesh = StaticMesh.LoadSynchronous()))
		{
			MeshComponent->SetStaticMesh();
		}
	}
	
	MovementComponent = CreateDefaultSubobject<USnakeMovementComponent>(TEXT("MovementComponent"));
	MovementComponent->SetUpdatedComponent(RootComponent);
	
	CameraSpringComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringComponent"));
	CameraSpringComponent->SetupAttachment(RootComponent);
	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(CameraSpringComponent);
}

// Called when the game starts or when spawned
void ASnakePawn::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Log, TEXT("SnakePawn::BeginPlay()!"));
}

// Called every frame
void ASnakePawn::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ASnakePawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UPawnMovementComponent* ASnakePawn::GetMovementComponent() const
{
	return MovementComponent;
}

float ASnakePawn::GetMovementSpeed() const
{
	return MovementSpeed;
}

