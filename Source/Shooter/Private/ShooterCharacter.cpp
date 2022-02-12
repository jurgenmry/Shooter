// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Animation/AnimMontage.h"
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


	//Dont allow the character to rotate when the controller does it 
	//Let the controller only affect the camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;


	//Allow the character to move only with the direction
	//Confirgure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in direction of movement
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.0f;
	GetCharacterMovement()->AirControl = 0.0f;

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

void AShooterCharacter::FireWeapon()
{
	//UE_LOG(LogTemp, Warning, TEXT("Fire Weapon"));
	
	if (FireSound)
	{
		UGameplayStatics::PlaySound2D(this, FireSound);
	} 

	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");
	if (BarrelSocket)
	{
		const FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());

		if (MuzzleFlash)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
		}
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && HipFireMontage)
	{
		AnimInstance->Montage_Play(HipFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
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

	//For the firing of weapons
	PlayerInputComponent->BindAction("FireButton", IE_Pressed, this, &AShooterCharacter::FireWeapon);
}