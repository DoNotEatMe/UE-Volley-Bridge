// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPlayer.h"
#include "Components/CapsuleComponent.h"

#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "AITypes.h"
#include "VolleyGameModeCPP.h"


// Sets default values
AAIPlayer::AAIPlayer()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AIControllerClass = AAIController::StaticClass();


	// Mesh setup
	Cylinder = CreateDefaultSubobject<UStaticMeshComponent>(FName("Cylinder"));
	if (Cylinder)
	{
		Cylinder->SetupAttachment(GetCapsuleComponent());
		static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderFind(
			TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
		if (CylinderFind.Succeeded())
		{
			Cylinder->SetStaticMesh(CylinderFind.Object);
			Cylinder->SetRelativeScale3D(FVector(0.6, 0.6, 1.25));
			Cylinder->SetRelativeLocation(FVector(0, 0, -25));
			Cylinder->SetCanEverAffectNavigation(false);
			static ConstructorHelpers::FObjectFinder<UMaterial> MaterialFind(
				TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
			if (MaterialFind.Succeeded())
			{
				Cylinder->SetMaterial(0, MaterialFind.Object);
			}
		}
	}
}

// Called when the game starts or when spawned
void AAIPlayer::BeginPlay()
{
	Super::BeginPlay();

	AIController = Cast<AAIController>(GetController());

	AIController->GetPathFollowingComponent()->OnRequestFinished.AddUObject(this, &AAIPlayer::PathCompleted);
	AIStartLocation = GetActorLocation();
}

// Called every frame
void AAIPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AAIPlayer::MoveTo(const FVector DestLocation)
{
	if (AIController)
	{
		AIController->MoveToLocation(DestLocation, 0.f);
	}
}


void AAIPlayer::PathCompleted(FAIRequestID RequestID, const FPathFollowingResult& Result)
{

	
	if (UWorld* World = GetWorld())
	{
		AVolleyGameModeCPP* GameMode = Cast<AVolleyGameModeCPP>(World->GetAuthGameMode());
		if (GameMode)
		{
			if (GameMode->FirstBall)
			{
				GameMode->FirstBall = false;

				//UE_LOG(LogTemp, Warning, TEXT("Ball spawned, %s"), *this->GetName());
				FVector SpawnPoint = GetActorLocation();
				SpawnPoint.Z += 100.f;
				GameMode->SpawnBall(SpawnPoint, this);

				MoveTo(AIStartLocation);
			}
		}
	}

	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Move competed, %s"), *this->GetName());
	}
}


void AAIPlayer::SetTeam(uint8 TeamNumber)
{
	Team = TeamNumber;
}
