// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"


// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;



	//Creation Of Camera Boom (Pulls in towards the character in case of collision)
	CameraBoom = CreateDefaultSubobject <USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f;			//Define the distance with the Root component
	CameraBoom->bUsePawnControlRotation = true;		//Use the rotation of the controllet for the Boom



	//Create A Follow Camera

	MyCamera = CreateDefaultSubobject <UCameraComponent>(TEXT("MyCamera"));
	MyCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attacht the camera at socket
	MyCamera->bUsePawnControlRotation = true; //camera does not rotate relative to arm


	BaseTurnRate = 45.0f;
	BaseLookUpRate = 45.0f;

}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AShooterCharacter::MoveForward(float Value)
{
	if ((Controller)&& Value != 0)
	{
		//Find Out wich way if forward
		const FRotator Rotation = Controller->GetControlRotation();     // Gets the Rotations of the controller
		const FRotator YawRotation = FRotator(0.0f, Rotation.Yaw, 0.0f);// Direction of the yaw

		const FVector Direction = FRotationMatrix{ YawRotation }.GetUnitAxis(EAxis::X);

		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if ((Controller) && Value != 0)
	{
		//Find Out wich way if Right
		const FRotator Rotation = Controller->GetControlRotation();     // Gets the Rotations of the controller
		const FRotator YawRotation = FRotator(0.0f, Rotation.Yaw, 0.0f);// Direction of the yaw

		const FVector Direction = FRotationMatrix{ YawRotation }.GetUnitAxis(EAxis::Y);

		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookUpRate(float Rate)
{
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	check(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);


	//For controller u other Key
	PlayerInputComponent->BindAxis("TurnRightRate", this, &AShooterCharacter::TurnAtRate); 
	PlayerInputComponent->BindAxis("LookUpRate", this, &AShooterCharacter::LookUpRate);

	//For the Mouse Movement
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

}