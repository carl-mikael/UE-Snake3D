// Fill out your copyright notice in the Description page of Project Settings.


#include "SnakePawn.h"

#include "Food.h"
#include "SnakeBodyCell.h"
#include "SnakeGameMode.h"
#include "SnakeMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASnakePawn::ASnakePawn()
{
	bReplicates = true;
	PrimaryActorTick.bCanEverTick = true;

	const FName HeadMeshAssetPath = TEXT("/Engine/BasicShapes/Cone.Cone");
	HeadMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeadMesh"));
    HeadMesh->SetStaticMesh(ConstructorHelpers::FObjectFinder<UStaticMesh>(*HeadMeshAssetPath.ToString()).Object);
	HeadMesh->SetNotifyRigidBodyCollision(true);
	HeadMesh->bReplicatePhysicsToAutonomousProxy = false;
	SetRootComponent(HeadMesh);
	
	Movement = CreateDefaultSubobject<USnakeMovementComponent>(TEXT("Movement"));
	Movement->SetUpdatedComponent(HeadMesh);
	
	CameraSpring = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpring"));
	CameraSpring->SetupAttachment(RootComponent);
	CameraSpring->SocketOffset = FVector(0.0f, 0.0f, 400.0f);
	CameraSpring->TargetArmLength = 85.0f;
	CameraSpring->bEnableCameraLag = true;
	CameraSpring->bEnableCameraRotationLag = true;
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraSpring);
	const FVector CameraLocation = FVector(-100.0f, 0.0f, 0.0f);
	Camera->AddLocalOffset(CameraLocation);
	const FRotator CameraRotation = FRotator(-35.0f, 0.0f, 0.0f);
	Camera->AddLocalRotation(CameraRotation);
	
	BodyCellActorClass = ASnakeBodyCell::StaticClass();
		
	NrOfBodyCells = 2;
	BodyCellOffset = 100.0f;
	MovementSpeed = 400.0f;
}

void ASnakePawn::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ASnakePawn, NrOfBodyCells);
}

void ASnakePawn::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	UE_LOG(LogTemp, Log, TEXT("SnakePawn::OnConstruction()!"));
}

// Called when the game starts or when spawned
void ASnakePawn::BeginPlay()
{
	Super::BeginPlay();
	
	UE_LOG(LogTemp, Log, TEXT("SnakePawn::BeginPlay()!"));
	
	if (HasAuthority())
	{
		if (ASnakeGameMode* SnakeGameMode = Cast<ASnakeGameMode>(GetWorld()->GetAuthGameMode()))
		{
			SnakeGameMode->RegisterSnakePawn(this);
		}
	}
	
	HeadMesh->OnComponentHit.AddUniqueDynamic(this, &ASnakePawn::OnHit);

	if (IsValid(BodyCellActorClass))
	{
		for (int i = 0; i < NrOfBodyCells; ++i)
		{
			AddBodyCell();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SnakePawn - ChildActorClass ptr invalid"));
	}
}

void ASnakePawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	HeadMesh->OnComponentHit.RemoveDynamic(this, &ASnakePawn::OnHit);
}

void ASnakePawn::Server_AddBodyCell_Implementation()
{
	NrOfBodyCells++;
	// APlayerState* PState = GetPlayerState();
	// if (IsValid(PState))
	// {
	// 	PState->SetScore(PState->GetScore() + 1);
	// 	UE_LOG(LogTemp, Log, TEXT("SnakePawn::Server_AddBodyCell_Implementation() - New Score: %f"), PState->GetScore());
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("SnakePawn::Server_AddBodyCell_Implementation() - PlayerSnakeState is invalid"));
	// }
	
	Multicast_AddBodyCell();
}

void ASnakePawn::Multicast_AddBodyCell_Implementation()
{
	AddBodyCell();
}

void ASnakePawn::AddBodyCell()
{
	const int BodyCellCount = BodyCellActors.Num();
	const FString BodyCellName = FString::Printf(TEXT("BodyCell%d"), BodyCellCount);	
	UChildActorComponent* ChildActorComponent = NewObject<UChildActorComponent>(this, *BodyCellName);
	ChildActorComponent->RegisterComponent();
	ChildActorComponent->SetChildActorClass(BodyCellActorClass);
	//ChildActorComponent->AttachToComponent(HeadMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	ChildActorComponents.Add(ChildActorComponent);
	const AActor* ChildActor = ChildActorComponent->GetChildActor();
	if (!IsValid(ChildActor))
	{
		UE_LOG(LogTemp, Error, TEXT("SnakePawn::BeginPlay() - Failed to spawn body cell actor i: %i"), BodyCellCount);
		return;
	}
	
	const AActor* LastBodyCell = !BodyCellActors.IsEmpty()? BodyCellActors.Last() : this;
	
	BodyCellActors.Add(ChildActorComponent->GetChildActor());
	
	const FVector Offset = LastBodyCell->GetActorForwardVector() * -BodyCellOffset;
	const FVector BodyCellWorldLocation = LastBodyCell->GetRootComponent()->GetComponentLocation() + Offset;
	ChildActor->GetRootComponent()->SetWorldLocation(BodyCellWorldLocation);
}

void ASnakePawn::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                       FVector NormalImpulse, const FHitResult& Hit)
{
	if (!IsLocallyControlled())
	{
		return;
	}
	
	Server_OnHit(OtherActor);

	// Food Collision
	AFood* FoodActor = Cast<AFood>(OtherActor);
	if (IsValid(FoodActor))
	{
		UE_LOG(LogTemp, Log, TEXT("SnakePawn::OnHit - Food!"));
		this->DestroyActor(FoodActor);
		Server_AddBodyCell();
	}

	// HeadCollision
	const AActor* SnakePawn = Cast<ASnakePawn>(OtherActor);
	if (IsValid(SnakePawn))
	{
		UE_LOG(LogTemp, Log, TEXT("SnakePawn::OnHit - Hit SnakePawnHead!"));
		this->DestroyActor(this);
	}

	// BodyCellCollision
	const AActor* BodyCellActor = Cast<ASnakeBodyCell>(OtherActor);
	if (IsValid(BodyCellActor))
	{
		const bool bIsSelf = (OtherActor->GetParentActor() == this);
		const FString Msg = bIsSelf ? TEXT("self") : TEXT("other");
		UE_LOG(LogTemp, Log, TEXT("SnakePawn::OnHit - Hit %s BodyCellActor!"), *Msg);
		this->DestroyActor(this);
	}
}

void ASnakePawn::DestroyActor(AActor* Actor) const
{
	Actor->SetHidden(true);
	Actor->SetActorEnableCollision(false);
	Actor->SetActorTickEnabled(false);
	Server_Destroy(Actor);
}

void ASnakePawn::Server_SendTransform_Implementation(const FVector NewLocation, const float DeltaTime)
{
	Multicast_UpdateTransform(NewLocation, DeltaTime);
}

void ASnakePawn::Multicast_UpdateTransform_Implementation(const FVector NewLocation, const float DeltaTime)
{
	if (IsLocallyControlled())
	{
		return;
	}
	
	const FVector Smoothed = FMath::VInterpTo(
		GetActorLocation(),
		NewLocation,
		DeltaTime,
		10.0f
	);

	SetActorLocation(Smoothed);
}

void ASnakePawn::Server_Destroy_Implementation(AActor* Actor) const
{
	Actor->Destroy();
}

void ASnakePawn::MoveBodyCells(const float DeltaTime)
{
	for (int i=0; i<BodyCellActors.Num(); ++i)
	{
		const AActor* BodyCell = BodyCellActors[i];
		if (!IsValid(BodyCell))
		{	
			UE_LOG(LogTemp, Error, TEXT("SnakePawn::Tick() - Body cell invalid i: %i"), i);
			break;
		}
		
		FVector PrevCellLocation = i==0? HeadMesh->GetComponentLocation() : BodyCellActors[i-1]->GetActorLocation();
		const FVector ToPrevCell = PrevCellLocation - BodyCell->GetActorLocation();
		const FRotator RotToPrevCell = ToPrevCell.Rotation();
		USceneComponent* CellRoot = BodyCell->GetRootComponent();
		CellRoot->SetWorldRotation(RotToPrevCell);
		FVector Move = FVector(MovementSpeed * DeltaTime, 0.0f, 0.0f);
		FVector NewLocation = BodyCell->GetActorLocation() + Move;
		const float DistanceSqr = (PrevCellLocation - NewLocation).Length();
		if (DistanceSqr < BodyCellOffset)
		{
			Move.X -= BodyCellOffset - DistanceSqr;
		}
		
		CellRoot->AddLocalOffset(Move);
	}
}

void ASnakePawn::Server_OnHit_Implementation(AActor* OtherActor)
{
	OnSneakHit.Broadcast(this, OtherActor);
}

// Called every frame
void ASnakePawn::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsLocallyControlled())
	{
		Server_SendTransform(GetActorLocation(), DeltaTime);
	}
	
	MoveBodyCells(DeltaTime);
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

