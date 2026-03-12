// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakePawn.h"

#include "SnakeMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ASnakePawn::ASnakePawn()
{
	PrimaryActorTick.bCanEverTick = true;

	DummyRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(DummyRoot);
	
	UChildActorComponent* NewChildActorComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("NewChildActor"));
	ChildActors.Emplace(NewChildActorComponent);
	NewChildActorComponent->SetupAttachment(RootComponent);
	
	HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetupAttachment(RootComponent);
	
	Movement = CreateDefaultSubobject<USnakeMovementComponent>(TEXT("Movement"));
	Movement->SetUpdatedComponent(RootComponent);
	
	CameraSpring = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpring"));
	CameraSpring->SetupAttachment(RootComponent);
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraSpring);
}

void ASnakePawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	UE_LOG(LogTemp, Log, TEXT("SnakePawn::OnConstruction"));
	if (IsValid(ChildActorClass))
	{
		for (const auto ChildActorComponent : ChildActors)
		{
			ChildActorComponent->SetChildActorClass(ChildActorClass);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SnakePawn - ChildActorClass ptr invalid"));
	}
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
	return Movement;
}

float ASnakePawn::GetMovementSpeed() const
{
	return MovementSpeed;
}

