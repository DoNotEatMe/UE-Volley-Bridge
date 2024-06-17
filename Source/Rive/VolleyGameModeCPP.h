// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "VolleyGameModeCPP.generated.h"

/**
 * 
 */
UCLASS()
class RIVE_API AVolleyGameModeCPP : public AGameModeBase
{
	GENERATED_BODY()

	/** MappingContext for player input. */
	
	

	public:

	AVolleyGameModeCPP();
	
	UPROPERTY()
	APlayerController* PlayerController;

	UPROPERTY()
	class APlayerPawnCPP* PlayerPawn;
	
	UPROPERTY()
	TArray<AActor*> AIArray;
 
	UPROPERTY()
	uint8 Team;

	UPROPERTY()
	TArray<class AAIPlayer*> TeamA;

	UPROPERTY()
	TArray<AAIPlayer*> TeamB;

	UPROPERTY()
	FVector AIPlayerLocation;

	UFUNCTION()
	void SpawnBall(FVector Location, AAIPlayer* Initiator);

	UPROPERTY()
	bool FirstBall = true;

	UFUNCTION()
	AAIPlayer* RandomMember(uint8 TeamNumber);

	// UI functionality

	UPROPERTY()
	UUserWidget* PrepareWBP;

	UFUNCTION()
	void PrepareToCatch();

	UFUNCTION()
	void RemovePrepare();

	UPROPERTY();
	UUserWidget* CountWBP;
	
	UFUNCTION()
	void UpdateCount();

	UPROPERTY()
	int BounceCount;

	UPROPERTY()
	int FailCount;

	UPROPERTY()
	int MaxBounce;

	UPROPERTY()
	int MaxFails;

	UFUNCTION()
	void EndGame(bool IsWin);

	UPROPERTY()
	UUserWidget* WinLoseWBP;
	
	

private:
	UFUNCTION()
	void RoundStart();

	UPROPERTY()
	FTimerHandle RoundStartTimer;

	

	protected:
	virtual void BeginPlay() override;
	
};
