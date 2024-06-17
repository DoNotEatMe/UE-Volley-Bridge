// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPawnCPP.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "VolleyGameModeCPP.h"
#include "Camera/CameraComponent.h"

// Sets default values
APlayerPawnCPP::APlayerPawnCPP()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//camera setup
	Camera = CreateDefaultSubobject<UCameraComponent>(FName("Camera"));
	if (Camera)
	{
		Camera->AddRelativeRotation(FRotator(-50, 0, 0));
		RootComponent = Camera;
	}

	// IA IMC setup
	static ConstructorHelpers::FObjectFinder<UInputMappingContext> IMC_MappingFind(
		TEXT("/Game/Rive/Volleyball/IA/IMC_main.IMC_main"));
	if (IMC_MappingFind.Succeeded())
	{
		InputMapping = IMC_MappingFind.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> IA_SpaceFind(
		TEXT("/Game/Rive/Volleyball/IA/IA_Space.IA_Space"));
	if (IA_SpaceFind.Succeeded())
	{
		IA_Space = IA_SpaceFind.Object;
	}
}

// Called when the game starts or when spawned
void APlayerPawnCPP::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}
}

// Called every frame
void APlayerPawnCPP::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APlayerPawnCPP::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(IA_Space, ETriggerEvent::Triggered, this, &APlayerPawnCPP::PressSpace);
	}
}

void APlayerPawnCPP::PressSpace(const FInputActionValue& Value)
{
	if (CouldHitBall)
	{
		WasSpacePressed = true;
		if (AVolleyGameModeCPP* GameMode = Cast<AVolleyGameModeCPP>(GetWorld()->GetAuthGameMode()))
		{
			GameMode->RemovePrepare();
		}
	}
}
