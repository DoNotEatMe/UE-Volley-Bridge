// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIPlayer.generated.h"

UCLASS()
class RIVE_API AAIPlayer : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAIPlayer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	

	UFUNCTION()
	void MoveTo(const FVector DestLocation);

	UFUNCTION()
	void SetTeam (uint8 TeamNumber);
	
	UPROPERTY()
	class UStaticMeshComponent* Cylinder;

	UPROPERTY(EditAnywhere, Category= "Serve Point")
	class ATargetPoint* ServeStart;

	

	UPROPERTY()
	uint8 Team;

	UPROPERTY()
	FVector AIStartLocation;


private:
	
	UPROPERTY()
	class AAIController* AIController;
	
	

	void PathCompleted(struct FAIRequestID RequestID, const struct FPathFollowingResult& Result);
	
};
