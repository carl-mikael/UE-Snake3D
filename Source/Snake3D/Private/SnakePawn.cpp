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
	
	HeadMeshAssetPath = TEXT("/Engine/BasicShapes/Cone.Cone");
	
	HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadMesh"));
    HeadMesh->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(*HeadMeshAssetPath.ToString()).Object);
	HeadMesh->SetNotifyRigidBodyCollision(true);
	HeadMesh->SetupAttachment(RootComponent);
	
	Movement = CreateDefaultSubobject<USnakeMovementComponent>(TEXT("Movement"));
	Movement->SetUpdatedComponent(RootComponent);
	
	CameraSpring = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpring"));
	CameraSpring->SetupAttachment(RootComponent);
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraSpring);
	
	BodyMeshAssetPath = TEXT("/Engine/BasicShapes/Cube.Cube");
	
	BodyCells.Add(CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyCell1")));
	
	MovementSpeed = 100.0f;
}

// Called when the game starts or when spawned
void ASnakePawn::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Log, TEXT("SnakePawn::BeginPlay()!"));
	HeadMesh->OnComponentHit.AddDynamic(this, &ASnakePawn::OnHit);
}

void ASnakePawn::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Log, TEXT("SnakePawn::OnHit()"));
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

