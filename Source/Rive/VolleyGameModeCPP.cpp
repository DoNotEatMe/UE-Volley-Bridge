// Fill out your copyright notice in the Description page of Project Settings.


#include "VolleyGameModeCPP.h"

#include "AIPlayer.h"
#include "PlayerPawnCPP.h"
#include "Engine/TargetPoint.h"
#include "Kismet/GameplayStatics.h"
#include "Ball.h"
#include "Blueprint/UserWidget.h"
#include "Misc/OutputDeviceNull.h"

AVolleyGameModeCPP::AVolleyGameModeCPP()
{
	DefaultPawnClass = APlayerPawnCPP::StaticClass();

	FString WidgetPath = TEXT("/Game/Rive/Volleyball/UI/WBP_Count.WBP_Count_C");
	UClass* WidgetCount = LoadObject<UClass>(nullptr, *WidgetPath);
	if (WidgetCount)
	{
		CountWBP = CreateWidget<UUserWidget>(GetWorld(), WidgetCount);
		if (CountWBP)
		{
			CountWBP->AddToViewport();
		}
	}

	WidgetPath = TEXT("/Game/Rive/Volleyball/UI/WBP_PlayerPrepare.WBP_PlayerPrepare_C");
	UClass* WidgetPrepare = LoadObject<UClass>(nullptr, *WidgetPath);
	if (WidgetPrepare)
	{
		PrepareWBP = CreateWidget<UUserWidget>(GetWorld(), WidgetPrepare);
	}

	WidgetPath = TEXT("/Game/Rive/Volleyball/UI/WBP_WinLose.WBP_WinLose_C");
	UClass* WidgetWinLose = LoadObject<UClass>(nullptr, *WidgetPath);
	if (WidgetWinLose)
	{
		WinLoseWBP = CreateWidget<UUserWidget>(GetWorld(), WidgetWinLose);
	}
}


void AVolleyGameModeCPP::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = GetWorld()->GetFirstPlayerController();
	FInputModeGameOnly Input;
	PlayerController->SetInputMode(Input);
	PlayerController->bShowMouseCursor = false;

	PlayerPawn = Cast<APlayerPawnCPP>(PlayerController->GetPawn());

	MaxBounce = 5;
	MaxFails = 5;

	if (UWorld* World = GetWorld())
	{
		UGameplayStatics::GetAllActorsOfClass(World, AAIPlayer::StaticClass(), AIArray);
	}

	for (AActor* Actor : AIArray)
	{
		if (AAIPlayer* AIPlayer = Cast<AAIPlayer>(Actor))
		{
			FVector AILocation = AIPlayer->GetActorLocation();
			if (AILocation.X > 0)
			{
				AIPlayer->SetTeam(0);
				TeamA.Add(AIPlayer);
			}
			if (AILocation.X < 0)
			{
				AIPlayer->SetTeam(1);
				TeamB.Add(AIPlayer);
			}
		}
	}

	if (!TeamA.IsEmpty() && TeamA.Num() == TeamB.Num())
	{
		AActor* ActorToTag = AIArray[FMath::RandRange(0, AIArray.Num() - 1)];
		ActorToTag->Tags.Add(TEXT("Player"));
		AAIPlayer* TaggedActor = Cast<AAIPlayer>(ActorToTag);

		UMaterial* Material = Cast<UMaterial>(StaticLoadObject(UMaterial::StaticClass(), nullptr,
		                                                       *FString(
			                                                       "/Engine/EngineDebugMaterials/VertexColorViewMode_GreenOnly.VertexColorViewMode_GreenOnly")));
		if (Material)
		{
			TaggedActor->Cylinder->SetMaterial(0, Material);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("thms goes wrong with team distribution"));
	}

	GetWorld()->GetTimerManager().SetTimer(RoundStartTimer, this, &AVolleyGameModeCPP::RoundStart, 2.0f, false);
}




void AVolleyGameModeCPP::RoundStart()
{
	if (FMath::RandBool())
	{
		for (AAIPlayer* AIPlayer : TeamA)
		{
			if (AIPlayer->ActorHasTag("Serve"))
			{
				UE_LOG(LogTemp, Warning, TEXT("RoundStarted"));
				AIPlayerLocation = AIPlayer->GetActorLocation();
				AIPlayer->MoveTo(AIPlayer->ServeStart->GetActorLocation());
				break;
			}
		}
	}
	else
	{
		for (AAIPlayer* AIPlayer : TeamB)
		{
			if (AIPlayer->ActorHasTag("Serve"))
			{
				AIPlayerLocation = AIPlayer->GetActorLocation();
				AIPlayer->MoveTo(AIPlayer->ServeStart->GetActorLocation());

				break;
			}
		}
	}
}

void AVolleyGameModeCPP::SpawnBall(FVector Location, AAIPlayer* Initiator)
{
	FActorSpawnParameters Params;
	FRotator SpawnRotation = FRotator::ZeroRotator;

	if (UWorld* World = GetWorld())
	{
		AActor* BallSpawn = World->SpawnActor(ABall::StaticClass(), &Location, &SpawnRotation, Params);
		ABall* Ball = Cast<ABall>(BallSpawn);
		if (Ball)
		{
			// Just want to save this form in case i'll be inspired to make game called "Квадрат"
			if (Initiator->Team == 0)
			{
				//UE_LOG(LogTemp,Warning,TEXT("Team %hhu Spawned Ball"), Initiator->Team)
				AAIPlayer* Receiver = TeamB[FMath::RandRange(0, TeamB.Num() - 1)];
				Ball->BallServe(Initiator, Receiver);
			}
			if (Initiator->Team == 1)
			{
				AAIPlayer* Receiver = TeamA[FMath::RandRange(0, TeamA.Num() - 1)];
				//UE_LOG(LogTemp,Warning,TEXT("Team %hhu Spawned Ball"), Initiator->Team)
				Ball->BallServe(Initiator, Receiver);
			}
		}
	}
}

AAIPlayer* AVolleyGameModeCPP::RandomMember(uint8 TeamNumber)
{
	if (TeamNumber == 0)
	{
		return TeamA[FMath::RandRange(0, TeamA.Num() - 1)];
	}
	if (TeamNumber == 1)
	{
		return TeamB[FMath::RandRange(0, TeamA.Num() - 1)];
	}

	return nullptr;
}

void AVolleyGameModeCPP::PrepareToCatch()
{
	if (PrepareWBP)
	{
		PrepareWBP->AddToViewport();
		PlayerPawn->CouldHitBall = true;
	}
}

void AVolleyGameModeCPP::RemovePrepare()
{
	PrepareWBP->RemoveFromParent();
}

void AVolleyGameModeCPP::UpdateCount()
{
	if (PlayerPawn->WasSpacePressed)
	{
		if (CountWBP)
		{
			BounceCount++;
			if (CountWBP->FindFunction(TEXT("SetBounce")))
			{
				const FString command = FString::Printf(TEXT("SetBounce %d"), BounceCount);
				FOutputDeviceNull ar;

				CountWBP->CallFunctionByNameWithArguments(*command, ar, NULL, true);

				PlayerPawn->WasSpacePressed = false;
			}
		}

		if (MaxBounce == BounceCount)
		{
			if (WinLoseWBP)
			{
				EndGame(true);
			}
		}
	}
	else
	{
		if (CountWBP)
		{
			FailCount++;
			if (CountWBP->FindFunction(TEXT("SetFailed")))
			{
				const FString command = FString::Printf(TEXT("SetFailed %d"), FailCount);
				FOutputDeviceNull ar;

				CountWBP->CallFunctionByNameWithArguments(*command, ar, NULL, true);
			}
		}

		if (MaxFails == FailCount)
		{
			EndGame(false);
		}
	}
}


void AVolleyGameModeCPP::EndGame(bool IsWin)
{
	const FString command = FString::Printf(TEXT("SetIsWin %d"), IsWin);
	FOutputDeviceNull ar;
	WinLoseWBP->CallFunctionByNameWithArguments(*command, ar, NULL, true);
				

	
	APlayerController* PawnController = Cast<APlayerController>(PlayerPawn->GetController());
	if (PawnController)
	{
		PawnController->SetShowMouseCursor(true);
		FInputModeUIOnly Input;
		PlayerController->SetInputMode(Input);
					
	}

	UGameplayStatics::SetGamePaused(GetWorld(),true);

	if (WinLoseWBP)
	{
		WinLoseWBP->AddToViewport();
	}

	
}