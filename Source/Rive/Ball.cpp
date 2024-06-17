// Fill out your copyright notice in the Description page of Project Settings.


#include "Ball.h"

#include "VolleyGameModeCPP.h"

// Sets default values
ABall::ABall()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Sphere = CreateDefaultSubobject<UStaticMeshComponent>(FName("Sphere"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereFind(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereFind.Succeeded())
	{
		Sphere->SetStaticMesh(SphereFind.Object);
		Sphere->SetRelativeScale3D(FVector(0.21, 0.21, 0.21));
		Sphere->SetSimulatePhysics(true);
		Sphere->SetMassOverrideInKg(NAME_None, 0.26);
		static ConstructorHelpers::FObjectFinder<UMaterialInstance> MaterialFind(
			TEXT("/Engine/VREditor/BasicMeshes/MI_Cube_03.MI_Cube_03"));
		if (MaterialFind.Succeeded())
		{
			Sphere->SetMaterial(0, MaterialFind.Object);
		}
	}
	RootComponent = Sphere;
	Sphere->SetNotifyRigidBodyCollision(true);
}

// Called when the game starts or when spawned
void ABall::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
	{
		GameMode = Cast<AVolleyGameModeCPP>(World->GetAuthGameMode());
		if (!GameMode)
		{
			UE_LOG(LogTemp, Warning, TEXT("Ball GameMode missing in begin play"));
		}
	}

	Sphere->OnComponentHit.AddDynamic(this, &ABall::OnHit);
}

// Called every frame
void ABall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABall::BallServe(AAIPlayer* SenderF, AAIPlayer* ReceiverF)
{
	UE_LOG(LogTemp,Warning,TEXT("Ball started from %s to %s"), *SenderF->GetName(), *ReceiverF->GetName() );
	Sender = SenderF;
	Receiver = ReceiverF;

	// If our AI was moved to ball, we need to return here
	AIStartPos = Receiver->AIStartLocation;

	// Player just staying
	if (Receiver->ActorHasTag(FName("Player")))
	{
		TargetPos = Receiver->GetActorLocation();
		GameMode->PrepareToCatch();
	}
	else
	// Ball pick random point near AI and then AI moves to the ball
	{
		TargetPos = Receiver->GetActorLocation();
		TargetPos.X += FMath::RandRange(0.f, 100.f);
		TargetPos.Y += FMath::RandRange(0.f, 100.f);
		//DrawDebugSphere(GetWorld(), TargetPos, 100.f, 10, FColor::Red, false, 50.f);
		Receiver->MoveTo(TargetPos);
	}

	// Ball position before impulse
	SpherePos = Sphere->GetComponentLocation();

	// Peak of the flying curve
	PeakHeight = NetHeight + FMath::RandRange(150, 200);

	// Height difference before Net and after
	HeightDifferenceAscent = PeakHeight - SpherePos.Z;
	HeightDifferenceDescent = PeakHeight - TargetPos.Z;

	if (HeightDifferenceAscent > 0 && HeightDifferenceDescent > 0)
	{
		TimeToPeak = FMath::Sqrt(2 * HeightDifferenceAscent / Gravity);
		TimeToDescend = FMath::Sqrt(2 * HeightDifferenceDescent / Gravity);
		TotalTimeOfFlight = TimeToPeak + TimeToDescend;

		float DistanceX = TargetPos.X - SpherePos.X;
		float DistanceY = TargetPos.Y - SpherePos.Y;

		float Vx = DistanceX / TotalTimeOfFlight;
		float Vy = DistanceY / TotalTimeOfFlight;
		float Vz = Gravity * TimeToPeak;

		FVector Impulse = FVector(Vx, Vy, Vz) * Sphere->GetMass();

		Sphere->AddImpulse(Impulse);
	}
}


void ABall::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                  FVector NormalImpulse, const FHitResult& Hit)
{
	Receiver = Cast<AAIPlayer>(OtherActor);
	if (Receiver)
	{
		if (LastHitActor != Receiver)
		{
			if (Receiver->ActorHasTag("Player"))
			{
				GameMode->UpdateCount();
				//UE_LOG(LogTemp, Warning, TEXT("PlayerHitted"));
				GameMode->RemovePrepare();
			}

			Sphere->SetPhysicsLinearVelocity(FVector::ZeroVector);
			Sphere->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
			//Sphere->SetSimulatePhysics(false);
			//Sphere->SetSimulatePhysics(true);

			Receiver->MoveTo(AIStartPos);


			Sender = Receiver;

			UWorld* World = GetWorld();
			if (World)
			{
				if (GameMode)
				{
					if (Sender->Team == 0)
					{
						AAIPlayer* RandomReceiver = GameMode->RandomMember(1);
						if (RandomReceiver)
						{
							BallServe(Sender, RandomReceiver);
						}
					}
					if (Sender->Team == 1)
					{
						AAIPlayer* RandomReceiver = GameMode->RandomMember(0);
						if (RandomReceiver)
						{
							BallServe(Sender, RandomReceiver);
						}
					}
				}
			}


			LastHitActor = Sender;
		}
	}
}
