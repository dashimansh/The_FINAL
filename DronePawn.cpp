#include "DronePawn.h"
#include "DroneHUD.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/PointLightComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/Engine.h"
#include "Math/UnrealMathUtility.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"

ADronePawn::ADronePawn()
{
	PrimaryActorTick.bCanEverTick = true;

	PhysicsBody = CreateDefaultSubobject<UStaticMeshComponent>(
		TEXT("PhysicsBody"));
	RootComponent = PhysicsBody;
	PhysicsBody->SetSimulatePhysics(false);
	PhysicsBody->SetEnableGravity(false);
	PhysicsBody->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	PhysicsBody->SetCollisionProfileName(TEXT("NoCollision"));
	PhysicsBody->SetVisibility(false);

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(
		TEXT("CollisionSphere"));
	CollisionSphere->SetupAttachment(RootComponent);
	CollisionSphere->SetSphereRadius(80.f);
	CollisionSphere->SetCollisionEnabled(
		ECollisionEnabled::QueryAndPhysics);
	CollisionSphere->SetCollisionProfileName(TEXT("Pawn"));
	CollisionSphere->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionSphere->SetCollisionResponseToChannel(
		ECC_WorldStatic, ECR_Block);
	CollisionSphere->SetCollisionResponseToChannel(
		ECC_WorldDynamic, ECR_Block);
	CollisionSphere->SetVisibility(false);
	CollisionSphere->SetHiddenInGame(true);
	CollisionSphere->OnComponentHit.AddDynamic(
		this, &ADronePawn::OnDroneHit);

	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(
		TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(RootComponent);
	BodyMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BodyMesh->SetSimulatePhysics(false);
	BodyMesh->SetRelativeScale3D(FVector(100.f));

	FL_PropellerRoot = CreateDefaultSubobject<USceneComponent>(
		TEXT("FL_PropellerRoot"));
	FL_PropellerRoot->SetupAttachment(RootComponent);
	FL_PropellerRoot->SetRelativeLocation(FVector(45.f, -45.f, 8.f));
	FL_PropellerPivot = CreateDefaultSubobject<USceneComponent>(
		TEXT("FL_PropellerPivot"));
	FL_PropellerPivot->SetupAttachment(FL_PropellerRoot);
	FL_PropellerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(
		TEXT("FL_PropellerMesh"));
	FL_PropellerMesh->SetupAttachment(FL_PropellerPivot);
	FL_PropellerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FL_PropellerMesh->SetSimulatePhysics(false);
	FL_PropellerMesh->SetRelativeScale3D(FVector(100.f));

	FR_PropellerRoot = CreateDefaultSubobject<USceneComponent>(
		TEXT("FR_PropellerRoot"));
	FR_PropellerRoot->SetupAttachment(RootComponent);
	FR_PropellerRoot->SetRelativeLocation(FVector(45.f, 45.f, 8.f));
	FR_PropellerPivot = CreateDefaultSubobject<USceneComponent>(
		TEXT("FR_PropellerPivot"));
	FR_PropellerPivot->SetupAttachment(FR_PropellerRoot);
	FR_PropellerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(
		TEXT("FR_PropellerMesh"));
	FR_PropellerMesh->SetupAttachment(FR_PropellerPivot);
	FR_PropellerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	FR_PropellerMesh->SetSimulatePhysics(false);
	FR_PropellerMesh->SetRelativeScale3D(FVector(100.f));

	BL_PropellerRoot = CreateDefaultSubobject<USceneComponent>(
		TEXT("BL_PropellerRoot"));
	BL_PropellerRoot->SetupAttachment(RootComponent);
	BL_PropellerRoot->SetRelativeLocation(FVector(-45.f, -45.f, 8.f));
	BL_PropellerPivot = CreateDefaultSubobject<USceneComponent>(
		TEXT("BL_PropellerPivot"));
	BL_PropellerPivot->SetupAttachment(BL_PropellerRoot);
	BL_PropellerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(
		TEXT("BL_PropellerMesh"));
	BL_PropellerMesh->SetupAttachment(BL_PropellerPivot);
	BL_PropellerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BL_PropellerMesh->SetSimulatePhysics(false);
	BL_PropellerMesh->SetRelativeScale3D(FVector(100.f));

	BR_PropellerRoot = CreateDefaultSubobject<USceneComponent>(
		TEXT("BR_PropellerRoot"));
	BR_PropellerRoot->SetupAttachment(RootComponent);
	BR_PropellerRoot->SetRelativeLocation(FVector(-45.f, 45.f, 8.f));
	BR_PropellerPivot = CreateDefaultSubobject<USceneComponent>(
		TEXT("BR_PropellerPivot"));
	BR_PropellerPivot->SetupAttachment(BR_PropellerRoot);
	BR_PropellerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(
		TEXT("BR_PropellerMesh"));
	BR_PropellerMesh->SetupAttachment(BR_PropellerPivot);
	BR_PropellerMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BR_PropellerMesh->SetSimulatePhysics(false);
	BR_PropellerMesh->SetRelativeScale3D(FVector(100.f));

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(
		TEXT("SpringArm"));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 300.f;
	SpringArm->SetRelativeRotation(FRotator(-20.f, 0.f, 0.f));
	SpringArm->bDoCollisionTest = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritRoll = false;
	SpringArm->bInheritYaw = true;

	ThirdPersonCamera = CreateDefaultSubobject<UCameraComponent>(
		TEXT("ThirdPersonCamera"));
	ThirdPersonCamera->SetupAttachment(SpringArm);
	ThirdPersonCamera->SetActive(true);

	FPVCamera = CreateDefaultSubobject<UCameraComponent>(
		TEXT("FPVCamera"));
	FPVCamera->SetupAttachment(RootComponent);
	FPVCamera->SetRelativeLocation(FVector(20.f, 0.f, 5.f));
	FPVCamera->SetRelativeRotation(FRotator(-10.f, 0.f, 0.f));
	FPVCamera->SetActive(false);

	GimbalRoot = CreateDefaultSubobject<USceneComponent>(
		TEXT("GimbalRoot"));
	GimbalRoot->SetupAttachment(RootComponent);
	GimbalRoot->SetRelativeLocation(FVector(15.f, 0.f, -10.f));

	GimbalYaw = CreateDefaultSubobject<USceneComponent>(
		TEXT("GimbalYaw"));
	GimbalYaw->SetupAttachment(GimbalRoot);

	GimbalPitch = CreateDefaultSubobject<USceneComponent>(
		TEXT("GimbalPitch"));
	GimbalPitch->SetupAttachment(GimbalYaw);
	GimbalPitch->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));

	GimbalCamera = CreateDefaultSubobject<UCameraComponent>(
		TEXT("GimbalCamera"));
	GimbalCamera->SetupAttachment(GimbalPitch);
	GimbalCamera->SetActive(false);
	GimbalCamera->FieldOfView = 80.f;

	PiPCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(
		TEXT("PiPCapture"));
	PiPCapture->SetupAttachment(RootComponent);
	PiPCapture->SetRelativeLocation(FVector(-250.f, 0.f, 300.f));
	PiPCapture->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
	PiPCapture->bCaptureEveryFrame = true;
	PiPCapture->bCaptureOnMovement = true;
	PiPCapture->FOVAngle = 90.f;
	PiPCapture->CaptureSource = SCS_FinalColorHDR;
	PiPCapture->PrimitiveRenderMode =
		ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;

	GimbalViewCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(
		TEXT("GimbalViewCapture"));
	GimbalViewCapture->SetupAttachment(GimbalPitch);
	GimbalViewCapture->bCaptureEveryFrame = true;
	GimbalViewCapture->bCaptureOnMovement = true;
	GimbalViewCapture->FOVAngle = 80.f;
	GimbalViewCapture->CaptureSource = SCS_FinalColorHDR;
	GimbalViewCapture->PrimitiveRenderMode =
		ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;

	Light_FL = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light_FL"));
	Light_FL->SetupAttachment(RootComponent);
	Light_FL->SetRelativeLocation(FVector(45.f, -45.f, 5.f));
	Light_FL->SetLightColor(FLinearColor(1.f, 0.f, 0.f));
	Light_FL->Intensity = 8000.f;
	Light_FL->AttenuationRadius = 200.f;
	Light_FL->bUseInverseSquaredFalloff = false;
	Light_FL->LightFalloffExponent = 4.f;
	Light_FL->SetCastShadows(false);

	Light_FR = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light_FR"));
	Light_FR->SetupAttachment(RootComponent);
	Light_FR->SetRelativeLocation(FVector(45.f, 45.f, 5.f));
	Light_FR->SetLightColor(FLinearColor(0.f, 1.f, 0.f));
	Light_FR->Intensity = 8000.f;
	Light_FR->AttenuationRadius = 200.f;
	Light_FR->bUseInverseSquaredFalloff = false;
	Light_FR->LightFalloffExponent = 4.f;
	Light_FR->SetCastShadows(false);

	Light_BL = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light_BL"));
	Light_BL->SetupAttachment(RootComponent);
	Light_BL->SetRelativeLocation(FVector(-45.f, -45.f, 5.f));
	Light_BL->SetLightColor(FLinearColor(1.f, 0.f, 0.f));
	Light_BL->Intensity = 8000.f;
	Light_BL->AttenuationRadius = 200.f;
	Light_BL->bUseInverseSquaredFalloff = false;
	Light_BL->LightFalloffExponent = 4.f;
	Light_BL->SetCastShadows(false);

	Light_BR = CreateDefaultSubobject<UPointLightComponent>(TEXT("Light_BR"));
	Light_BR->SetupAttachment(RootComponent);
	Light_BR->SetRelativeLocation(FVector(-45.f, 45.f, 5.f));
	Light_BR->SetLightColor(FLinearColor(0.f, 1.f, 0.f));
	Light_BR->Intensity = 8000.f;
	Light_BR->AttenuationRadius = 200.f;
	Light_BR->bUseInverseSquaredFalloff = false;
	Light_BR->LightFalloffExponent = 4.f;
	Light_BR->SetCastShadows(false);

	Light_Front = CreateDefaultSubobject<UPointLightComponent>(
		TEXT("Light_Front"));
	Light_Front->SetupAttachment(RootComponent);
	Light_Front->SetRelativeLocation(FVector(60.f, 0.f, 5.f));
	Light_Front->SetLightColor(FLinearColor(1.f, 1.f, 1.f));
	Light_Front->Intensity = 10000.f;
	Light_Front->AttenuationRadius = 300.f;
	Light_Front->bUseInverseSquaredFalloff = false;
	Light_Front->LightFalloffExponent = 4.f;
	Light_Front->SetCastShadows(false);

	Light_Body = CreateDefaultSubobject<UPointLightComponent>(
		TEXT("Light_Body"));
	Light_Body->SetupAttachment(RootComponent);
	Light_Body->SetRelativeLocation(FVector(0.f, 0.f, -5.f));
	Light_Body->SetLightColor(FLinearColor(0.f, 1.f, 1.f));
	Light_Body->Intensity = 6000.f;
	Light_Body->AttenuationRadius = 250.f;
	Light_Body->bUseInverseSquaredFalloff = false;
	Light_Body->LightFalloffExponent = 3.f;
	Light_Body->SetCastShadows(false);

	PiPRenderTarget = nullptr;
	GimbalViewRenderTarget = nullptr;
	DroneHUD = nullptr;
	LightBlinkTimer = 0.f;
	bLightsOn = true;
	BodyPulseTimer = 0.f;
	FrontStrobeTimer = 0.f;
	bArmLightsEnabled = true;
	bBodyLightsEnabled = true;
	bBraking = false;
	bLoiterMode = false;
	bRTLMode = false;
	bRTLFollowingPath = false;
	bRTLHovering = false;
	bWaypointMode = false;
	bCrashed = false;
	bGimbalActive = false;
	bShowTrail = false;
	bMissionMode = false;
	CurrentMissionIndex = 0;
	CurrentWaypointIndex = 0;
	CurrentRTLIndex = 0;
	GimbalCurrentFOV = 80.f;
}

void ADronePawn::BeginPlay()
{
	Super::BeginPlay();

	FVector StartPos = GetActorLocation();
	HomePosition = StartPos;
	LoiterPosition = StartPos;
	TargetAltitude = StartPos.Z;
	PID_Alt.Reset();

	ThirdPersonCamera->SetActive(true);
	FPVCamera->SetActive(false);
	GimbalCamera->SetActive(false);

	bFPVMode = false;
	bJoystickMode = false;
	bBraking = false;
	bLoiterMode = false;
	bRTLMode = false;
	bRTLFollowingPath = false;
	bRTLHovering = false;
	bWaypointMode = false;
	bCrashed = false;
	bGimbalActive = false;
	bArmLightsEnabled = true;
	bBodyLightsEnabled = true;
	bShowTrail = false;
	bMissionMode = false;
	CurrentMissionIndex = 0;
	CurrentWaypointIndex = 0;
	CurrentRTLIndex = 0;
	CurrentGimbalPitch = GimbalPitchAngle;
	CurrentGimbalYaw = 0.f;
	GimbalYawAngle = 0.f;
	GimbalCurrentFOV = 80.f;
	LightBlinkTimer = 0.f;
	bLightsOn = true;
	BodyPulseTimer = 0.f;
	FrontStrobeTimer = 0.f;

	Input_Throttle = 0.f;
	Input_Pitch = 0.f;
	Input_Roll = 0.f;
	Input_Yaw = 0.f;
	Target_Throttle = 0.f;
	Target_Pitch = 0.f;
	Target_Roll = 0.f;
	Target_Yaw = 0.f;
	DroneVelocity = FVector::ZeroVector;
	CurrentPitch = 0.f;
	CurrentRoll = 0.f;
	SmoothMotor_FL = 0.f;
	SmoothMotor_FR = 0.f;
	SmoothMotor_BL = 0.f;
	SmoothMotor_BR = 0.f;

	LastRecordedPos = StartPos;
	FlightPath.Reserve(MaxPathPoints);
	FlightPath.Add(StartPos);

	CollisionSphere->SetSphereRadius(SphereRadius);

	if (!PiPRenderTarget)
	{
		PiPRenderTarget = NewObject<UTextureRenderTarget2D>(this);
		PiPRenderTarget->RenderTargetFormat = RTF_RGBA16f;
		PiPRenderTarget->ClearColor = FLinearColor::Black;
		PiPRenderTarget->bAutoGenerateMips = false;
		PiPRenderTarget->InitAutoFormat(512, 512);
		PiPRenderTarget->UpdateResourceImmediate(true);
	}
	if (PiPCapture)
	{
		PiPCapture->TextureTarget = PiPRenderTarget;
		PiPCapture->CaptureSource = SCS_FinalColorHDR;
		PiPCapture->bCaptureEveryFrame = true;
		PiPCapture->bCaptureOnMovement = true;
		PiPCapture->PrimitiveRenderMode =
			ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;
		PiPCapture->ShowFlags.SetAtmosphere(true);
		PiPCapture->ShowFlags.SetFog(true);
		PiPCapture->ShowFlags.SetBSP(true);
		PiPCapture->ShowFlags.SetStaticMeshes(true);
		PiPCapture->ShowFlags.SetSkeletalMeshes(true);
		PiPCapture->ShowFlags.SetLighting(true);
		PiPCapture->ShowFlags.SetPostProcessing(true);
		PiPCapture->ShowFlags.SetDynamicShadows(true);
		PiPCapture->ShowFlags.SetSkyLighting(true);
		PiPCapture->CaptureScene();
	}

	if (!GimbalViewRenderTarget)
	{
		GimbalViewRenderTarget = NewObject<UTextureRenderTarget2D>(this);
		GimbalViewRenderTarget->RenderTargetFormat = RTF_RGBA16f;
		GimbalViewRenderTarget->ClearColor = FLinearColor::Black;
		GimbalViewRenderTarget->bAutoGenerateMips = false;
		GimbalViewRenderTarget->InitAutoFormat(512, 512);
		GimbalViewRenderTarget->UpdateResourceImmediate(true);
	}
	if (GimbalViewCapture)
	{
		GimbalViewCapture->TextureTarget = GimbalViewRenderTarget;
		GimbalViewCapture->CaptureSource = SCS_FinalColorHDR;
		GimbalViewCapture->bCaptureEveryFrame = true;
		GimbalViewCapture->bCaptureOnMovement = true;
		GimbalViewCapture->PrimitiveRenderMode =
			ESceneCapturePrimitiveRenderMode::PRM_RenderScenePrimitives;
		GimbalViewCapture->ShowFlags.SetAtmosphere(true);
		GimbalViewCapture->ShowFlags.SetFog(true);
		GimbalViewCapture->ShowFlags.SetBSP(true);
		GimbalViewCapture->ShowFlags.SetStaticMeshes(true);
		GimbalViewCapture->ShowFlags.SetSkeletalMeshes(true);
		GimbalViewCapture->ShowFlags.SetLighting(true);
		GimbalViewCapture->ShowFlags.SetPostProcessing(true);
		GimbalViewCapture->ShowFlags.SetDynamicShadows(true);
		GimbalViewCapture->ShowFlags.SetSkyLighting(true);
		GimbalViewCapture->CaptureScene();
	}

	APlayerController* PC = Cast<APlayerController>(GetController());
	if (PC)
	{
		DroneHUD = Cast<ADroneHUD>(PC->GetHUD());
		if (DroneHUD)
		{
			if (PiPRenderTarget)
				DroneHUD->SetPiPRenderTarget(PiPRenderTarget);
			if (GimbalViewRenderTarget)
				DroneHUD->SetGimbalRenderTarget(GimbalViewRenderTarget);
		}
	}

	if (GEngine)
		GEngine->AddOnScreenDebugMessage(5, 5.f, FColor::Yellow,
			TEXT("P=AddWaypoint O=StartMission K=Clear "
				"W/S/A/D/E/Q/Z/X=Fly "
				"G=Gimbal B=MissionPanel "
				"H=HUD M=Map R=RTL"));
}

void ADronePawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bCrashed) return;

	CurrentAltitude = GetActorLocation().Z / 100.f;

	if (bMissionMode)
		ApplyMissionFlight(DeltaTime);
	else if (bWaypointMode)
		ApplyWaypointFlight(DeltaTime);
	else if (bLoiterMode)
		ApplyLoiterFlight(DeltaTime);
	else if (bRTLMode)
		ApplyRTLFlight(DeltaTime);
	else
		ApplyManualFlight(DeltaTime);

	UpdateMotorValues();
	SpinPropellers(DeltaTime);
	UpdateGimbal(DeltaTime);
	UpdateLights(DeltaTime);
	UpdateHUD();

	if (bShowTrail)
	{
		RecordFlightPath();
		DrawFlightTrail();
	}

	DrawWaypoints();
}

float ADronePawn::ComputePID(
	FPIDState& State, float Kp, float Ki, float Kd,
	float Error, float DeltaTime)
{
	float P = Kp * Error;
	State.Integral += Error * DeltaTime;
	State.Integral = FMath::Clamp(State.Integral, -50.f, 50.f);
	float Derivative = (DeltaTime > 0.f)
		? (Error - State.PrevError) / DeltaTime : 0.f;
	State.PrevError = Error;
	return P + (Ki * State.Integral) + (Kd * Derivative);
}

void ADronePawn::ApplyManualFlight(float DeltaTime)
{
	FVector Pos = GetActorLocation();

	FRotator YawOnly = FRotator(0.f, GetActorRotation().Yaw, 0.f);
	FVector  Fwd = FRotationMatrix(YawOnly).GetScaledAxis(EAxis::X);
	FVector  Rgt = FRotationMatrix(YawOnly).GetScaledAxis(EAxis::Y);

	if (FMath::Abs(Input_Throttle) > 0.01f)
	{
		DroneVelocity.Z += Input_Throttle * ThrottleSpeed * DeltaTime;
		DroneVelocity.Z = FMath::Clamp(DroneVelocity.Z,
			-ThrottleSpeed, ThrottleSpeed);
	}
	else
		DroneVelocity.Z *= 0.85f;

	bool bHasPitch = FMath::Abs(Input_Pitch) > 0.05f;
	bool bHasRoll = FMath::Abs(Input_Roll) > 0.05f;

	if (bHasPitch)
	{
		CurrentPitch = FMath::FInterpTo(CurrentPitch,
			-Input_Pitch * MaxTiltAngle, DeltaTime, 8.f);
		DroneVelocity += Fwd * Input_Pitch * PitchSpeed * DeltaTime;
	}
	else
		CurrentPitch = FMath::FInterpTo(
			CurrentPitch, 0.f, DeltaTime, 6.f);

	if (bHasRoll)
	{
		CurrentRoll = FMath::FInterpTo(CurrentRoll,
			Input_Roll * MaxTiltAngle, DeltaTime, 8.f);
		DroneVelocity += Rgt * Input_Roll * RollSpeed * DeltaTime;
	}
	else
		CurrentRoll = FMath::FInterpTo(
			CurrentRoll, 0.f, DeltaTime, 6.f);

	if (!bHasPitch && !bHasRoll)
	{
		FVector H(DroneVelocity.X, DroneVelocity.Y, 0.f);
		H *= FMath::Clamp(1.f - 4.f * DeltaTime, 0.f, 1.f);
		DroneVelocity.X = H.X;
		DroneVelocity.Y = H.Y;
	}

	FRotator NewRot = GetActorRotation();
	NewRot.Pitch = CurrentPitch;
	NewRot.Roll = CurrentRoll;
	NewRot.Yaw += Input_Yaw * YawSpeed * DeltaTime;
	SetActorRotation(NewRot);

	FVector H(DroneVelocity.X, DroneVelocity.Y, 0.f);
	const float MaxH = 2500.f;
	if (H.Size() > MaxH)
	{
		H = H.GetSafeNormal() * MaxH;
		DroneVelocity.X = H.X;
		DroneVelocity.Y = H.Y;
	}

	FVector NewPos = Pos + DroneVelocity * DeltaTime;
	CheckGroundCollision(NewPos);
	FHitResult Hit;
	if (!SetActorLocation(NewPos, true, &Hit) && Hit.bBlockingHit)
		HandleCollision(Hit, Hit.ImpactNormal);
}

void ADronePawn::ApplyLoiterFlight(float DeltaTime)
{
	FVector Pos = GetActorLocation();
	FVector Error = LoiterPosition - Pos;
	DroneVelocity.X = FMath::FInterpTo(DroneVelocity.X,
		Error.X * 8.f, DeltaTime, 5.f);
	DroneVelocity.Y = FMath::FInterpTo(DroneVelocity.Y,
		Error.Y * 8.f, DeltaTime, 5.f);
	float AltOut = ComputePID(PID_Alt, Alt_Kp, Alt_Ki, Alt_Kd,
		LoiterPosition.Z - Pos.Z, DeltaTime);
	DroneVelocity.Z = FMath::Clamp(AltOut, -300.f, 300.f);
	CurrentPitch = FMath::FInterpTo(CurrentPitch, 0.f, DeltaTime, 5.f);
	CurrentRoll = FMath::FInterpTo(CurrentRoll, 0.f, DeltaTime, 5.f);
	FRotator R = GetActorRotation();
	R.Pitch = CurrentPitch; R.Roll = CurrentRoll;
	SetActorRotation(R);
	FVector NewPos = Pos + DroneVelocity * DeltaTime;
	CheckGroundCollision(NewPos);
	FHitResult Hit;
	if (!SetActorLocation(NewPos, true, &Hit) && Hit.bBlockingHit)
		HandleCollision(Hit, Hit.ImpactNormal);
}

void ADronePawn::ApplyRTLFlight(float DeltaTime)
{
	FVector Pos = GetActorLocation();

	if (bRTLFollowingPath)
	{
		if (RTLPath.Num() == 0 || CurrentRTLIndex >= RTLPath.Num())
		{
			bRTLFollowingPath = false;
			bRTLHovering = true;
			DroneVelocity = FVector::ZeroVector;
			LoiterPosition = HomePosition;
			PID_Alt.Reset();
			if (GEngine) GEngine->AddOnScreenDebugMessage(
				11, 3.f, FColor::Green,
				TEXT("RTL: HOME REACHED"));
			return;
		}

		FVector Target = RTLPath[CurrentRTLIndex];
		FVector ToTarget = Target - Pos;
		float   Dist = ToTarget.Size();

		FVector ToTargetH(ToTarget.X, ToTarget.Y, 0.f);
		if (ToTargetH.SizeSquared() > 1.f)
		{
			float TargetYaw = FMath::RadiansToDegrees(
				FMath::Atan2(ToTargetH.Y, ToTargetH.X));
			FRotator R = GetActorRotation();
			R.Yaw = FMath::FInterpTo(
				R.Yaw, TargetYaw, DeltaTime, 4.f);
			R.Pitch = FMath::FInterpTo(R.Pitch, 0.f, DeltaTime, 4.f);
			R.Roll = FMath::FInterpTo(R.Roll, 0.f, DeltaTime, 4.f);
			SetActorRotation(R);
		}

		if (Dist > RTLAcceptRadius)
		{
			FVector Dir = ToTarget.GetSafeNormal();
			float   Spd = FMath::Min(Dist * 0.5f, RTLSpeed);
			DroneVelocity.X = FMath::FInterpTo(
				DroneVelocity.X, Dir.X * Spd, DeltaTime, 4.f);
			DroneVelocity.Y = FMath::FInterpTo(
				DroneVelocity.Y, Dir.Y * Spd, DeltaTime, 4.f);
			DroneVelocity.Z = FMath::Clamp(
				(Target.Z - Pos.Z) * 0.5f, -RTLSpeed, RTLSpeed);
		}
		else
		{
			CurrentRTLIndex++;
		}
	}
	else if (bRTLHovering)
	{
		FVector Err = LoiterPosition - Pos;
		DroneVelocity.X = FMath::FInterpTo(
			DroneVelocity.X, Err.X * 8.f, DeltaTime, 5.f);
		DroneVelocity.Y = FMath::FInterpTo(
			DroneVelocity.Y, Err.Y * 8.f, DeltaTime, 5.f);
		float AltOut = ComputePID(PID_Alt, Alt_Kp, Alt_Ki, Alt_Kd,
			LoiterPosition.Z - Pos.Z, DeltaTime);
		DroneVelocity.Z = FMath::Clamp(AltOut, -300.f, 300.f);
		CurrentPitch = FMath::FInterpTo(
			CurrentPitch, 0.f, DeltaTime, 5.f);
		CurrentRoll = FMath::FInterpTo(
			CurrentRoll, 0.f, DeltaTime, 5.f);
		FRotator R = GetActorRotation();
		R.Pitch = CurrentPitch; R.Roll = CurrentRoll;
		SetActorRotation(R);
	}

	FVector NewPos = Pos + DroneVelocity * DeltaTime;
	CheckGroundCollision(NewPos);
	FHitResult Hit;
	if (!SetActorLocation(NewPos, true, &Hit) && Hit.bBlockingHit)
		HandleCollision(Hit, Hit.ImpactNormal);
}

// ---------------------------------------------------------------------------
// Waypoint Flight — flies to EXACT X,Y,Z where drone was when P was pressed
// ---------------------------------------------------------------------------

void ADronePawn::ApplyWaypointFlight(float DeltaTime)
{
	if (Waypoints.Num() == 0) { bWaypointMode = false; return; }

	FVector Pos = GetActorLocation();
	FVector Target = Waypoints[CurrentWaypointIndex]; // full 3D target
	FVector ToTarget = Target - Pos;
	float   Dist = ToTarget.Size();

	// Face direction of travel
	FVector ToTargetH(ToTarget.X, ToTarget.Y, 0.f);
	if (ToTargetH.SizeSquared() > 1.f)
	{
		float TargetYaw = FMath::RadiansToDegrees(
			FMath::Atan2(ToTargetH.Y, ToTargetH.X));
		FRotator R = GetActorRotation();
		R.Yaw = FMath::FInterpTo(R.Yaw, TargetYaw, DeltaTime, 3.f);
		R.Pitch = FMath::FInterpTo(R.Pitch, 0.f, DeltaTime, 3.f);
		R.Roll = FMath::FInterpTo(R.Roll, 0.f, DeltaTime, 3.f);
		SetActorRotation(R);
	}

	if (Dist > WaypointAcceptRadius)
	{
		// Move toward full 3D target
		FVector Dir = ToTarget.GetSafeNormal();
		float   Spd = FMath::Min(Dist * 0.5f, WaypointSpeed);

		DroneVelocity.X = FMath::FInterpTo(
			DroneVelocity.X, Dir.X * Spd, DeltaTime, 3.f);
		DroneVelocity.Y = FMath::FInterpTo(
			DroneVelocity.Y, Dir.Y * Spd, DeltaTime, 3.f);

		// Z moves to exact target altitude
		DroneVelocity.Z = FMath::Clamp(
			(Target.Z - Pos.Z) * 0.5f,
			-WaypointSpeed, WaypointSpeed);
	}
	else
	{
		// Reached this waypoint
		if (GEngine) GEngine->AddOnScreenDebugMessage(
			13, 2.f, FColor::Green,
			FString::Printf(
				TEXT(">> WAYPOINT [%d] REACHED <<"),
				CurrentWaypointIndex));

		if (CurrentWaypointIndex < Waypoints.Num() - 1)
		{
			CurrentWaypointIndex++;
			if (GEngine) GEngine->AddOnScreenDebugMessage(
				14, 2.f, FColor::Cyan,
				FString::Printf(
					TEXT(">> FLYING TO WAYPOINT [%d/%d] <<"),
					CurrentWaypointIndex + 1,
					Waypoints.Num()));
		}
		else
		{
			// All waypoints done — loiter at last one
			bWaypointMode = false;
			bLoiterMode = true;
			LoiterPosition = Target;
			DroneVelocity = FVector::ZeroVector;
			PID_Alt.Reset();
			if (GEngine) GEngine->AddOnScreenDebugMessage(
				13, 4.f, FColor::Green,
				TEXT(">> ALL WAYPOINTS COMPLETE — LOITERING <<"));
		}
	}

	FVector NewPos = Pos + DroneVelocity * DeltaTime;
	CheckGroundCollision(NewPos);
	FHitResult Hit;
	if (!SetActorLocation(NewPos, true, &Hit) && Hit.bBlockingHit)
		HandleCollision(Hit, Hit.ImpactNormal);
}

void ADronePawn::ApplyMissionFlight(float DeltaTime)
{
	if (!DroneHUD) return;
	const TArray<FVector>& WPs = DroneHUD->GetMissionWaypoints();

	if (WPs.Num() == 0)
	{
		bMissionMode = false;
		DroneHUD->SetMissionActive(false, 0);
		return;
	}

	FVector Pos = GetActorLocation();
	FVector Target = WPs[CurrentMissionIndex];
	FVector ToTarget = Target - Pos;
	float   DistTotal = ToTarget.Size();

	FVector ToTargetH(ToTarget.X, ToTarget.Y, 0.f);
	if (ToTargetH.SizeSquared() > 1.f)
	{
		float TargetYaw = FMath::RadiansToDegrees(
			FMath::Atan2(ToTargetH.Y, ToTargetH.X));
		FRotator R = GetActorRotation();
		R.Yaw = FMath::FInterpTo(R.Yaw, TargetYaw, DeltaTime, 4.f);
		R.Pitch = FMath::FInterpTo(R.Pitch, 0.f, DeltaTime, 4.f);
		R.Roll = FMath::FInterpTo(R.Roll, 0.f, DeltaTime, 4.f);
		SetActorRotation(R);
	}

	if (DistTotal > MissionAcceptRadius)
	{
		FVector Dir = ToTarget.GetSafeNormal();
		float   Spd = FMath::Min(DistTotal * 0.5f, MissionSpeed);
		DroneVelocity.X = FMath::FInterpTo(
			DroneVelocity.X, Dir.X * Spd, DeltaTime, 4.f);
		DroneVelocity.Y = FMath::FInterpTo(
			DroneVelocity.Y, Dir.Y * Spd, DeltaTime, 4.f);
		DroneVelocity.Z = FMath::Clamp(
			(Target.Z - Pos.Z) * 0.5f,
			-MissionSpeed, MissionSpeed);
	}
	else
	{
		if (CurrentMissionIndex < WPs.Num() - 1)
		{
			CurrentMissionIndex++;
			DroneHUD->SetMissionActive(true, CurrentMissionIndex);
		}
		else
		{
			bMissionMode = false;
			bLoiterMode = true;
			LoiterPosition = Target;
			DroneVelocity = FVector::ZeroVector;
			PID_Alt.Reset();
			DroneHUD->SetMissionActive(false, 0);
			if (GEngine) GEngine->AddOnScreenDebugMessage(
				25, 5.f, FColor::Green,
				TEXT(">> MISSION COMPLETE! <<"));
		}
	}

	for (int32 i = 0; i < WPs.Num(); i++)
	{
		FColor C = i < CurrentMissionIndex
			? FColor::Green
			: i == CurrentMissionIndex
			? FColor::Yellow : FColor(128, 0, 255);
		DrawDebugSphere(GetWorld(), WPs[i],
			200.f, 24, C, false, 0.05f, 0, 5.f);
		if (i < WPs.Num() - 1)
			DrawDebugLine(GetWorld(),
				WPs[i], WPs[i + 1],
				FColor(128, 0, 255), false, 0.05f, 0, 4.f);
		DrawDebugLine(GetWorld(),
			FVector(WPs[i].X, WPs[i].Y, 0.f),
			WPs[i], FColor(80, 0, 160), false, 0.05f, 0, 2.f);
		DrawDebugString(GetWorld(),
			WPs[i] + FVector(0.f, 0.f, 250.f),
			FString::Printf(TEXT("WP[%d]\nX:%.0f\nY:%.0f\nZ:%.0f"),
				i, WPs[i].X, WPs[i].Y, WPs[i].Z),
			nullptr, C, 0.05f);
	}

	FVector NewPos = Pos + DroneVelocity * DeltaTime;
	CheckGroundCollision(NewPos);
	FHitResult Hit;
	if (!SetActorLocation(NewPos, true, &Hit) && Hit.bBlockingHit)
		HandleCollision(Hit, Hit.ImpactNormal);
}

void ADronePawn::UpdateMotorValues()
{
	float T = FMath::Clamp(FMath::Abs(Input_Throttle), 0.08f, 1.f);
	float P = FMath::Clamp(Input_Pitch, -1.f, 1.f);
	float R = FMath::Clamp(Input_Roll, -1.f, 1.f);
	float Y = FMath::Clamp(Input_Yaw, -1.f, 1.f);
	Motor_FL = FMath::Clamp(T + P + R + Y, 0.f, 1.f);
	Motor_FR = FMath::Clamp(T + P - R - Y, 0.f, 1.f);
	Motor_BL = FMath::Clamp(T - P + R - Y, 0.f, 1.f);
	Motor_BR = FMath::Clamp(T - P - R + Y, 0.f, 1.f);
}

void ADronePawn::SpinPropellers(float DeltaTime)
{
	const float Idle = 0.08f;
	SmoothMotor_FL = FMath::FInterpTo(SmoothMotor_FL,
		FMath::Max(Motor_FL, Idle), DeltaTime, PropellerSmoothSpeed);
	SmoothMotor_FR = FMath::FInterpTo(SmoothMotor_FR,
		FMath::Max(Motor_FR, Idle), DeltaTime, PropellerSmoothSpeed);
	SmoothMotor_BL = FMath::FInterpTo(SmoothMotor_BL,
		FMath::Max(Motor_BL, Idle), DeltaTime, PropellerSmoothSpeed);
	SmoothMotor_BR = FMath::FInterpTo(SmoothMotor_BR,
		FMath::Max(Motor_BR, Idle), DeltaTime, PropellerSmoothSpeed);
	PropAngle_FL = FMath::Fmod(PropAngle_FL -
		(SmoothMotor_FL * MaxRotorSpeed * DeltaTime), 360.f);
	PropAngle_FR = FMath::Fmod(PropAngle_FR +
		(SmoothMotor_FR * MaxRotorSpeed * DeltaTime), 360.f);
	PropAngle_BL = FMath::Fmod(PropAngle_BL +
		(SmoothMotor_BL * MaxRotorSpeed * DeltaTime), 360.f);
	PropAngle_BR = FMath::Fmod(PropAngle_BR -
		(SmoothMotor_BR * MaxRotorSpeed * DeltaTime), 360.f);
	if (FL_PropellerPivot) FL_PropellerPivot->SetRelativeRotation(
		FRotator(0.f, PropAngle_FL, 0.f));
	if (FR_PropellerPivot) FR_PropellerPivot->SetRelativeRotation(
		FRotator(0.f, PropAngle_FR, 0.f));
	if (BL_PropellerPivot) BL_PropellerPivot->SetRelativeRotation(
		FRotator(0.f, PropAngle_BL, 0.f));
	if (BR_PropellerPivot) BR_PropellerPivot->SetRelativeRotation(
		FRotator(0.f, PropAngle_BR, 0.f));
}

void ADronePawn::UpdateGimbal(float DeltaTime)
{
	if (!GimbalCamera) return;
	CurrentGimbalPitch = FMath::FInterpTo(CurrentGimbalPitch,
		FMath::Clamp(GimbalPitchAngle, -90.f, 90.f),
		DeltaTime, GimbalSmoothSpeed);
	CurrentGimbalYaw = FMath::FInterpTo(CurrentGimbalYaw,
		GimbalYawAngle, DeltaTime, GimbalSmoothSpeed);
	if (GimbalYaw)
		GimbalYaw->SetWorldRotation(
			FRotator(0.f, CurrentGimbalYaw, 0.f));
	if (GimbalPitch)
		GimbalPitch->SetRelativeRotation(
			FRotator(CurrentGimbalPitch, 0.f, 0.f));
}

void ADronePawn::UpdateLights(float DeltaTime)
{
	if (bArmLightsEnabled)
	{
		LightBlinkTimer += DeltaTime;
		if (LightBlinkTimer >= 1.f / LightBlinkSpeed)
		{
			LightBlinkTimer = 0.f;
			bLightsOn = !bLightsOn;
			if (Light_FL) Light_FL->SetVisibility(bLightsOn);
			if (Light_FR) Light_FR->SetVisibility(bLightsOn);
			if (Light_BL) Light_BL->SetVisibility(bLightsOn);
			if (Light_BR) Light_BR->SetVisibility(bLightsOn);
		}
	}
	if (bBodyLightsEnabled)
	{
		BodyPulseTimer += DeltaTime * 2.f;
		float PulseAlpha = (FMath::Sin(BodyPulseTimer) + 1.f) * 0.5f;
		if (Light_Body)
			Light_Body->SetIntensity(
				FMath::Lerp(1000.f, LightIntensity, PulseAlpha));
		FrontStrobeTimer += DeltaTime;
		if (FrontStrobeTimer >= 0.4f)
		{
			FrontStrobeTimer = 0.f;
			if (Light_Front)
				Light_Front->SetVisibility(
					!Light_Front->IsVisible());
		}
	}
}

bool ADronePawn::GetGroundHeight(FVector Position, float& OutGroundZ)
{
	FHitResult Hit;
	FVector Start(Position.X, Position.Y, Position.Z + 100.f);
	FVector End(Position.X, Position.Y, Position.Z - 10000.f);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(
		Hit, Start, End, ECC_WorldStatic, Params))
	{
		OutGroundZ = Hit.ImpactPoint.Z;
		return true;
	}
	OutGroundZ = 0.f;
	return false;
}

void ADronePawn::CheckGroundCollision(FVector& NewPos)
{
	float GroundZ = 0.f;
	if (!GetGroundHeight(NewPos, GroundZ)) return;
	float MinHeight = GroundZ + 50.f;
	if (NewPos.Z < MinHeight)
	{
		if (FMath::Abs(DroneVelocity.Z) > CrashSpeedThreshold * 0.5f)
		{
			bCrashed = true;
			DroneVelocity = FVector::ZeroVector;
			Input_Throttle = Input_Pitch = Input_Roll = Input_Yaw = 0.f;
			if (GEngine)
				GEngine->AddOnScreenDebugMessage(30, 5.f, FColor::Red,
					TEXT(">> CRASHED! Resetting in 3s... <<"));
			GetWorldTimerManager().SetTimer(CrashTimerHandle,
				this, &ADronePawn::ResetCrash, 3.f, false);
		}
		else
		{
			NewPos.Z = MinHeight;
			DroneVelocity.Z = 0.f;
		}
	}
}

void ADronePawn::OnDroneHit(
	UPrimitiveComponent* HitComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	FVector NormalImpulse,
	const FHitResult& Hit)
{
	if (bCrashed) return;
	HandleCollision(Hit, Hit.ImpactNormal);
}

void ADronePawn::HandleCollision(
	const FHitResult& Hit, FVector ImpactNormal)
{
	float ImpactSpeed = DroneVelocity.Size();
	if (ImpactSpeed > CrashSpeedThreshold)
	{
		bCrashed = true;
		DroneVelocity = FVector::ZeroVector;
		Input_Throttle = Input_Pitch = Input_Roll = Input_Yaw = 0.f;
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(30, 5.f, FColor::Red,
				FString::Printf(
					TEXT(">> CRASHED! Speed: %.0f <<"),
					ImpactSpeed));
		GetWorldTimerManager().SetTimer(CrashTimerHandle,
			this, &ADronePawn::ResetCrash, 3.f, false);
	}
	else
	{
		FVector Reflected = FMath::GetReflectionVector(
			DroneVelocity.GetSafeNormal(), ImpactNormal);
		DroneVelocity = Reflected * ImpactSpeed * BounceForce;
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(30, 1.f,
				FColor::Yellow, TEXT(">> IMPACT <<"));
	}
}

void ADronePawn::ResetCrash()
{
	bCrashed = false;
	DroneVelocity = FVector::ZeroVector;
	Input_Throttle = Input_Pitch = Input_Roll = Input_Yaw = 0.f;
	float GroundZ = 0.f;
	FVector Pos = GetActorLocation();
	GetGroundHeight(Pos, GroundZ);
	SetActorLocation(FVector(Pos.X, Pos.Y, GroundZ + 300.f));
	FRotator R = GetActorRotation();
	R.Pitch = 0.f; R.Roll = 0.f;
	SetActorRotation(R);
	if (GEngine)
		GEngine->AddOnScreenDebugMessage(30, 3.f, FColor::Green,
			TEXT(">> DRONE RESET! FLY AGAIN! <<"));
}

void ADronePawn::ResetBrake() { bBraking = false; }

void ADronePawn::RecordFlightPath()
{
	FVector CP = GetActorLocation();
	if (FVector::Dist(CP, LastRecordedPos) < PathPointDistance) return;
	if (FlightPath.Num() >= MaxPathPoints) FlightPath.RemoveAt(0);
	FlightPath.Add(CP);
	LastRecordedPos = CP;
}

void ADronePawn::DrawFlightTrail()
{
	if (FlightPath.Num() < 2) return;
	for (int32 i = 1; i < FlightPath.Num(); i++)
	{
		FVector A = FlightPath[i - 1];
		FVector B = FlightPath[i];
		float   AN = FMath::Clamp(B.Z / 5000.f, 0.f, 1.f);
		FColor  TC = AN < 0.33f ? FColor::Cyan
			: AN < 0.66f ? FColor::Green : FColor::Red;
		DrawDebugLine(GetWorld(), A, B, TC,
			false, 0.1f, 0, TrailLineThickness);
	}
	if (bRTLMode && RTLPath.Num() > 1)
	{
		for (int32 i = CurrentRTLIndex + 1; i < RTLPath.Num(); i++)
			DrawDebugLine(GetWorld(),
				RTLPath[i - 1], RTLPath[i],
				FColor::Orange, false, 0.1f, 0, 4.f);
	}
}

void ADronePawn::ClearFlightPath()
{
	FlightPath.Empty();
	LastRecordedPos = GetActorLocation();
	FlushPersistentDebugLines(GetWorld());
}

// ---------------------------------------------------------------------------
// DrawWaypoints — spheres drawn at EXACT waypoint Z (drone level)
// ---------------------------------------------------------------------------

void ADronePawn::DrawWaypoints()
{
	if (Waypoints.Num() == 0) return;
	for (int32 i = 0; i < Waypoints.Num(); i++)
	{
		FVector WP = Waypoints[i]; // exact position including Z
		FColor  WPC = i < CurrentWaypointIndex ? FColor::Green
			: i == CurrentWaypointIndex ? FColor::Yellow
			: FColor::Cyan;

		// Sphere at exact drone level where waypoint was placed
		DrawDebugSphere(GetWorld(), WP, 120.f, 16,
			WPC, false, 0.05f, 0, 4.f);

		// Vertical line from ground to waypoint
		DrawDebugLine(GetWorld(),
			FVector(WP.X, WP.Y, 0.f), WP,
			WPC, false, 0.05f, 0, 2.f);

		// Label
		DrawDebugString(GetWorld(),
			WP + FVector(0.f, 0.f, 150.f),
			FString::Printf(
				TEXT("WP[%d]\nX:%.0f Y:%.0f Z:%.0f"),
				i, WP.X, WP.Y, WP.Z),
			nullptr, WPC, 0.05f);

		// Line to next waypoint
		if (i < Waypoints.Num() - 1)
			DrawDebugLine(GetWorld(),
				WP, Waypoints[i + 1],
				FColor::White, false, 0.05f, 0, 3.f);
	}
}

void ADronePawn::UpdateHUD()
{
	if (!DroneHUD) return;
	FVector Pos = GetActorLocation();
	DroneHUD->UpdateData(
		Motor_FL, Motor_FR, Motor_BL, Motor_BR,
		CurrentAltitude,
		Input_Throttle, Input_Pitch, Input_Roll, Input_Yaw,
		bFPVMode, bJoystickMode, bBraking,
		Pos.X, Pos.Y, Pos.Z,
		bLoiterMode, bRTLMode);
	DroneHUD->UpdateMinimapData(
		Pos, GetActorRotation(),
		CurrentGimbalYaw, bGimbalActive);
}

void ADronePawn::UpdateCameras()
{
	if (bGimbalActive)
	{
		ThirdPersonCamera->SetActive(false);
		FPVCamera->SetActive(false);
		GimbalCamera->SetActive(true);
	}
	else if (bFPVMode)
	{
		ThirdPersonCamera->SetActive(false);
		FPVCamera->SetActive(true);
		GimbalCamera->SetActive(false);
	}
	else
	{
		ThirdPersonCamera->SetActive(true);
		FPVCamera->SetActive(false);
		GimbalCamera->SetActive(false);
	}
}

void ADronePawn::SetupPlayerInputComponent(
	UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("ThrottleUp",
		this, &ADronePawn::OnThrottleUp);
	PlayerInputComponent->BindAxis("ThrottleDown",
		this, &ADronePawn::OnThrottleDown);
	PlayerInputComponent->BindAxis("Pitch",
		this, &ADronePawn::OnPitch);
	PlayerInputComponent->BindAxis("Roll",
		this, &ADronePawn::OnRoll);
	PlayerInputComponent->BindAxis("KeyThrottleUp",
		this, &ADronePawn::OnKeyThrottleUp);
	PlayerInputComponent->BindAxis("KeyThrottleDown",
		this, &ADronePawn::OnKeyThrottleDown);
	PlayerInputComponent->BindAxis("KeyPitch",
		this, &ADronePawn::OnKeyPitch);
	PlayerInputComponent->BindAxis("KeyRoll",
		this, &ADronePawn::OnKeyRoll);
	PlayerInputComponent->BindAxis("GimbalPitchUp",
		this, &ADronePawn::OnGimbalPitchUp);
	PlayerInputComponent->BindAxis("GimbalPitchDown",
		this, &ADronePawn::OnGimbalPitchDown);
	PlayerInputComponent->BindAxis("GimbalYawLeft",
		this, &ADronePawn::OnGimbalYawLeft);
	PlayerInputComponent->BindAxis("GimbalYawRight",
		this, &ADronePawn::OnGimbalYawRight);
	PlayerInputComponent->BindAction("YawLeft",
		IE_Pressed, this, &ADronePawn::OnYawLeft);
	PlayerInputComponent->BindAction("YawLeft",
		IE_Released, this, &ADronePawn::OnYawLeftReleased);
	PlayerInputComponent->BindAction("YawRight",
		IE_Pressed, this, &ADronePawn::OnYawRight);
	PlayerInputComponent->BindAction("YawRight",
		IE_Released, this, &ADronePawn::OnYawRightReleased);
	PlayerInputComponent->BindAction("KeyYawLeft",
		IE_Pressed, this, &ADronePawn::OnKeyYawLeft);
	PlayerInputComponent->BindAction("KeyYawLeft",
		IE_Released, this, &ADronePawn::OnKeyYawLeftReleased);
	PlayerInputComponent->BindAction("KeyYawRight",
		IE_Pressed, this, &ADronePawn::OnKeyYawRight);
	PlayerInputComponent->BindAction("KeyYawRight",
		IE_Released, this, &ADronePawn::OnKeyYawRightReleased);
	PlayerInputComponent->BindAction("ToggleCamera",
		IE_Pressed, this, &ADronePawn::OnToggleCamera);
	PlayerInputComponent->BindAction("ToggleInputMode",
		IE_Pressed, this, &ADronePawn::OnToggleInputMode);
	PlayerInputComponent->BindAction("ToggleGimbal",
		IE_Pressed, this, &ADronePawn::OnToggleGimbal);
	PlayerInputComponent->BindAction("ToggleHUD",
		IE_Pressed, this, &ADronePawn::OnToggleHUD);
	PlayerInputComponent->BindAction("ToggleMinimap",
		IE_Pressed, this, &ADronePawn::OnToggleMinimap);
	PlayerInputComponent->BindAction("ToggleArmLights",
		IE_Pressed, this, &ADronePawn::OnToggleArmLights);
	PlayerInputComponent->BindAction("ToggleBodyLights",
		IE_Pressed, this, &ADronePawn::OnToggleBodyLights);
	PlayerInputComponent->BindAction("ToggleMotorPanel",
		IE_Pressed, this, &ADronePawn::OnToggleMotorPanel);
	PlayerInputComponent->BindAction("ToggleFlightPanel",
		IE_Pressed, this, &ADronePawn::OnToggleFlightPanel);
	PlayerInputComponent->BindAction("ToggleModePanel",
		IE_Pressed, this, &ADronePawn::OnToggleModePanel);
	PlayerInputComponent->BindAction("ToggleInputPanel",
		IE_Pressed, this, &ADronePawn::OnToggleInputPanel);
	PlayerInputComponent->BindAction("ToggleTrail",
		IE_Pressed, this, &ADronePawn::OnToggleTrail);
	PlayerInputComponent->BindAction("ClearTrail",
		IE_Pressed, this, &ADronePawn::OnClearTrail);
	PlayerInputComponent->BindAction("Brake",
		IE_Pressed, this, &ADronePawn::OnBrake);
	PlayerInputComponent->BindAction("Loiter",
		IE_Pressed, this, &ADronePawn::OnLoiterToggle);
	PlayerInputComponent->BindAction("RTL",
		IE_Pressed, this, &ADronePawn::OnRTLToggle);
	PlayerInputComponent->BindAction("AddWaypoint",
		IE_Pressed, this, &ADronePawn::OnAddWaypoint);
	PlayerInputComponent->BindAction("StartWaypoint",
		IE_Pressed, this, &ADronePawn::OnStartWaypoint);
	PlayerInputComponent->BindAction("ClearWaypoints",
		IE_Pressed, this, &ADronePawn::OnClearWaypoints);
	PlayerInputComponent->BindAction("ToggleMissionPanel",
		IE_Pressed, this, &ADronePawn::OnToggleMissionPanel);
	PlayerInputComponent->BindAction("MissionFieldNext",
		IE_Pressed, this, &ADronePawn::OnMissionNextField);
	PlayerInputComponent->BindAction("MissionAddWaypoint",
		IE_Pressed, this, &ADronePawn::OnMissionAddWaypoint);
	PlayerInputComponent->BindAction("MissionClearAll",
		IE_Pressed, this, &ADronePawn::OnMissionClearAll);
	PlayerInputComponent->BindAction("StartMission",
		IE_Pressed, this, &ADronePawn::OnStartMission);
	PlayerInputComponent->BindAction("GimbalZoomIn",
		IE_Pressed, this, &ADronePawn::OnGimbalZoomIn);
	PlayerInputComponent->BindAction("GimbalZoomOut",
		IE_Pressed, this, &ADronePawn::OnGimbalZoomOut);
}

void ADronePawn::OnThrottleUp(float Value)
{
	if (!bJoystickMode || bLoiterMode || bRTLMode || bWaypointMode) return;
	Input_Throttle = FMath::Clamp(Input_Throttle +
		Value * GetWorld()->GetDeltaSeconds() * 2.f, -1.f, 1.f);
}
void ADronePawn::OnThrottleDown(float Value)
{
	if (!bJoystickMode || bLoiterMode || bRTLMode || bWaypointMode) return;
	Input_Throttle = FMath::Clamp(Input_Throttle -
		Value * GetWorld()->GetDeltaSeconds() * 2.f, -1.f, 1.f);
}
void ADronePawn::OnPitch(float Value)
{
	if (!bJoystickMode || bLoiterMode || bRTLMode || bWaypointMode) return;
	Input_Pitch = FMath::Clamp(Value, -1.f, 1.f);
}
void ADronePawn::OnRoll(float Value)
{
	if (!bJoystickMode || bLoiterMode || bRTLMode || bWaypointMode) return;
	Input_Roll = FMath::Clamp(Value, -1.f, 1.f);
}
void ADronePawn::OnYawLeft()
{
	if (!bJoystickMode || bLoiterMode || bRTLMode || bWaypointMode) return;
	Input_Yaw = -1.f;
}
void ADronePawn::OnYawLeftReleased()
{
	if (!bJoystickMode) return;
	if (Input_Yaw < 0.f) Input_Yaw = 0.f;
}
void ADronePawn::OnYawRight()
{
	if (!bJoystickMode || bLoiterMode || bRTLMode || bWaypointMode) return;
	Input_Yaw = 1.f;
}
void ADronePawn::OnYawRightReleased()
{
	if (!bJoystickMode) return;
	if (Input_Yaw > 0.f) Input_Yaw = 0.f;
}
void ADronePawn::OnKeyThrottleUp(float Value)
{
	if (bJoystickMode || bLoiterMode || bRTLMode || bWaypointMode) return;
	if (Value > 0.f)
		Input_Throttle = FMath::Clamp(Input_Throttle +
			Value * GetWorld()->GetDeltaSeconds() * 2.f, -1.f, 1.f);
	else
		Input_Throttle = FMath::FInterpTo(Input_Throttle,
			0.f, GetWorld()->GetDeltaSeconds(), 5.f);
}
void ADronePawn::OnKeyThrottleDown(float Value)
{
	if (bJoystickMode || bLoiterMode || bRTLMode || bWaypointMode) return;
	if (Value > 0.f)
		Input_Throttle = FMath::Clamp(Input_Throttle -
			Value * GetWorld()->GetDeltaSeconds() * 2.f, -1.f, 1.f);
	else
		Input_Throttle = FMath::FInterpTo(Input_Throttle,
			0.f, GetWorld()->GetDeltaSeconds(), 5.f);
}
void ADronePawn::OnKeyPitch(float Value)
{
	if (bJoystickMode || bLoiterMode || bRTLMode || bWaypointMode) return;
	Input_Pitch = FMath::Abs(Value) > 0.1f
		? FMath::Clamp(Value, -1.f, 1.f) : 0.f;
}
void ADronePawn::OnKeyRoll(float Value)
{
	if (bJoystickMode || bLoiterMode || bRTLMode || bWaypointMode) return;
	Input_Roll = FMath::Abs(Value) > 0.1f
		? FMath::Clamp(Value, -1.f, 1.f) : 0.f;
}
void ADronePawn::OnKeyYawLeft()
{
	if (bJoystickMode || bLoiterMode || bRTLMode || bWaypointMode) return;
	Input_Yaw = -1.f;
}
void ADronePawn::OnKeyYawLeftReleased()
{
	if (bJoystickMode) return;
	if (Input_Yaw < 0.f) Input_Yaw = 0.f;
}
void ADronePawn::OnKeyYawRight()
{
	if (bJoystickMode || bLoiterMode || bRTLMode || bWaypointMode) return;
	Input_Yaw = 1.f;
}
void ADronePawn::OnKeyYawRightReleased()
{
	if (bJoystickMode) return;
	if (Input_Yaw > 0.f) Input_Yaw = 0.f;
}
void ADronePawn::OnGimbalPitchUp(float Value)
{
	if (!bGimbalActive || Value <= 0.f) return;
	GimbalPitchAngle = FMath::Clamp(GimbalPitchAngle +
		Value * GetWorld()->GetDeltaSeconds() * GimbalRotateSpeed,
		-90.f, 90.f);
}
void ADronePawn::OnGimbalPitchDown(float Value)
{
	if (!bGimbalActive || Value <= 0.f) return;
	GimbalPitchAngle = FMath::Clamp(GimbalPitchAngle -
		Value * GetWorld()->GetDeltaSeconds() * GimbalRotateSpeed,
		-90.f, 90.f);
}
void ADronePawn::OnGimbalYawLeft(float Value)
{
	if (!bGimbalActive || Value <= 0.f) return;
	GimbalYawAngle -= Value * GetWorld()->GetDeltaSeconds()
		* GimbalRotateSpeed;
	if (GimbalYawAngle < -180.f) GimbalYawAngle += 360.f;
}
void ADronePawn::OnGimbalYawRight(float Value)
{
	if (!bGimbalActive || Value <= 0.f) return;
	GimbalYawAngle += Value * GetWorld()->GetDeltaSeconds()
		* GimbalRotateSpeed;
	if (GimbalYawAngle > 180.f) GimbalYawAngle -= 360.f;
}
void ADronePawn::OnGimbalZoomIn()
{
	if (!bGimbalActive) return;
	GimbalCurrentFOV = FMath::Clamp(
		GimbalCurrentFOV - GimbalZoomSpeed,
		GimbalMinFOV, GimbalMaxFOV);
	if (GimbalCamera)
		GimbalCamera->FieldOfView = GimbalCurrentFOV;
	if (GimbalViewCapture)
		GimbalViewCapture->FOVAngle = GimbalCurrentFOV;
	if (GEngine) GEngine->AddOnScreenDebugMessage(
		50, 1.f, FColor::Cyan,
		FString::Printf(TEXT("ZOOM: %.0f"), GimbalCurrentFOV));
}
void ADronePawn::OnGimbalZoomOut()
{
	if (!bGimbalActive) return;
	GimbalCurrentFOV = FMath::Clamp(
		GimbalCurrentFOV + GimbalZoomSpeed,
		GimbalMinFOV, GimbalMaxFOV);
	if (GimbalCamera)
		GimbalCamera->FieldOfView = GimbalCurrentFOV;
	if (GimbalViewCapture)
		GimbalViewCapture->FOVAngle = GimbalCurrentFOV;
	if (GEngine) GEngine->AddOnScreenDebugMessage(
		50, 1.f, FColor::Cyan,
		FString::Printf(TEXT("ZOOM: %.0f"), GimbalCurrentFOV));
}
void ADronePawn::OnToggleGimbal()
{
	bGimbalActive = !bGimbalActive;
	bFPVMode = false;
	UpdateCameras();
	if (bGimbalActive)
	{
		CurrentGimbalYaw = GetActorRotation().Yaw;
		GimbalYawAngle = CurrentGimbalYaw;
		CurrentGimbalPitch = GimbalPitchAngle;
		GimbalCurrentFOV = 80.f;
		if (GimbalCamera)
			GimbalCamera->FieldOfView = GimbalCurrentFOV;
		if (GimbalViewCapture)
			GimbalViewCapture->FOVAngle = GimbalCurrentFOV;
		if (GEngine) GEngine->AddOnScreenDebugMessage(
			40, 2.f, FColor::Cyan,
			TEXT(">> GIMBAL ON  I/K=Pitch  J/L=Yaw  Scroll=Zoom <<"));
	}
	else
		if (GEngine) GEngine->AddOnScreenDebugMessage(
			40, 2.f, FColor::White, TEXT(">> GIMBAL OFF <<"));
}
void ADronePawn::OnToggleHUD()
{
	if (DroneHUD) DroneHUD->ToggleHUD();
}
void ADronePawn::OnToggleMinimap()
{
	if (DroneHUD) DroneHUD->ToggleMinimap();
}
void ADronePawn::OnToggleArmLights()
{
	bArmLightsEnabled = !bArmLightsEnabled;
	if (!bArmLightsEnabled)
	{
		if (Light_FL) Light_FL->SetVisibility(false);
		if (Light_FR) Light_FR->SetVisibility(false);
		if (Light_BL) Light_BL->SetVisibility(false);
		if (Light_BR) Light_BR->SetVisibility(false);
		if (GEngine) GEngine->AddOnScreenDebugMessage(
			70, 2.f, FColor::Red, TEXT(">> ARM LIGHTS OFF <<"));
	}
	else
	{
		LightBlinkTimer = 0.f; bLightsOn = true;
		if (Light_FL) Light_FL->SetVisibility(true);
		if (Light_FR) Light_FR->SetVisibility(true);
		if (Light_BL) Light_BL->SetVisibility(true);
		if (Light_BR) Light_BR->SetVisibility(true);
		if (GEngine) GEngine->AddOnScreenDebugMessage(
			70, 2.f, FColor::Green, TEXT(">> ARM LIGHTS ON <<"));
	}
}
void ADronePawn::OnToggleBodyLights()
{
	bBodyLightsEnabled = !bBodyLightsEnabled;
	if (!bBodyLightsEnabled)
	{
		if (Light_Body)  Light_Body->SetVisibility(false);
		if (Light_Front) Light_Front->SetVisibility(false);
		if (GEngine) GEngine->AddOnScreenDebugMessage(
			71, 2.f, FColor::Red, TEXT(">> BODY LIGHTS OFF <<"));
	}
	else
	{
		if (Light_Body)  Light_Body->SetVisibility(true);
		if (Light_Front) Light_Front->SetVisibility(true);
		BodyPulseTimer = 0.f; FrontStrobeTimer = 0.f;
		if (GEngine) GEngine->AddOnScreenDebugMessage(
			71, 2.f, FColor::Cyan, TEXT(">> BODY LIGHTS ON <<"));
	}
}
void ADronePawn::OnToggleMotorPanel()
{
	if (DroneHUD) DroneHUD->ToggleMotorPanel();
}
void ADronePawn::OnToggleFlightPanel()
{
	if (DroneHUD) DroneHUD->ToggleFlightPanel();
}
void ADronePawn::OnToggleModePanel()
{
	if (DroneHUD) DroneHUD->ToggleModePanel();
}
void ADronePawn::OnToggleInputPanel()
{
	if (DroneHUD) DroneHUD->ToggleInputPanel();
}
void ADronePawn::OnToggleTrail()
{
	bShowTrail = !bShowTrail;
	if (!bShowTrail)
	{
		ClearFlightPath();
		if (GEngine) GEngine->AddOnScreenDebugMessage(
			90, 2.f, FColor::Red, TEXT(">> TRAIL OFF <<"));
	}
	else
	{
		LastRecordedPos = GetActorLocation();
		FlightPath.Empty();
		FlightPath.Add(GetActorLocation());
		if (GEngine) GEngine->AddOnScreenDebugMessage(
			90, 2.f, FColor::Cyan, TEXT(">> TRAIL ON <<"));
	}
}
void ADronePawn::OnClearTrail()
{
	ClearFlightPath();
	if (GEngine) GEngine->AddOnScreenDebugMessage(
		8, 2.f, FColor::White, TEXT(">> TRAIL CLEARED <<"));
}
void ADronePawn::OnBrake()
{
	if (bRTLMode) return;
	bBraking = true;
	bLoiterMode = false;
	bWaypointMode = false;
	DroneVelocity = FVector::ZeroVector;
	Input_Throttle = Input_Pitch = Input_Roll = Input_Yaw = 0.f;
	CurrentPitch = CurrentRoll = 0.f;
	FRotator R = GetActorRotation();
	R.Pitch = 0.f; R.Roll = 0.f;
	SetActorRotation(R);
	if (GEngine) GEngine->AddOnScreenDebugMessage(
		7, 1.5f, FColor::Yellow, TEXT("BRAKE APPLIED"));
	GetWorldTimerManager().SetTimer(BrakeTimerHandle,
		this, &ADronePawn::ResetBrake, 0.1f, false);
}
void ADronePawn::OnToggleCamera()
{
	if (bGimbalActive) { bGimbalActive = false; bFPVMode = false; }
	else bFPVMode = !bFPVMode;
	UpdateCameras();
	if (GEngine) GEngine->AddOnScreenDebugMessage(
		6, 2.f, FColor::Cyan,
		bGimbalActive ? TEXT(">> GIMBAL <<")
		: bFPVMode ? TEXT(">> FPV MODE <<")
		: TEXT(">> 3RD PERSON <<"));
}
void ADronePawn::OnToggleInputMode()
{
	bJoystickMode = !bJoystickMode;
	Input_Throttle = Input_Pitch = Input_Roll = Input_Yaw = 0.f;
	if (GEngine) GEngine->AddOnScreenDebugMessage(
		5, 3.f, FColor::Yellow,
		bJoystickMode
		? TEXT(">> JOYSTICK MODE <<")
		: TEXT(">> KEYBOARD MODE <<"));
}
void ADronePawn::OnLoiterToggle()
{
	if (bRTLMode || bWaypointMode) return;
	bLoiterMode = !bLoiterMode;
	if (bLoiterMode)
	{
		LoiterPosition = GetActorLocation();
		DroneVelocity = FVector::ZeroVector;
		Input_Throttle = Input_Pitch = Input_Roll = Input_Yaw = 0.f;
		PID_Alt.Reset();
		if (GEngine) GEngine->AddOnScreenDebugMessage(
			9, 3.f, FColor::Cyan, TEXT(">> LOITER ON <<"));
	}
	else
		if (GEngine) GEngine->AddOnScreenDebugMessage(
			9, 2.f, FColor::White, TEXT(">> LOITER OFF <<"));
}
void ADronePawn::OnRTLToggle()
{
	bRTLMode = !bRTLMode;
	if (bRTLMode)
	{
		bLoiterMode = false;
		bWaypointMode = false;
		RTLPath.Empty();
		RTLPath.Add(GetActorLocation());
		for (int32 i = FlightPath.Num() - 1; i >= 0; i--)
			RTLPath.Add(FlightPath[i]);
		CurrentRTLIndex = 0;
		bRTLFollowingPath = true;
		bRTLHovering = false;
		DroneVelocity = FVector::ZeroVector;
		Input_Throttle = Input_Pitch = Input_Roll = Input_Yaw = 0.f;
		PID_Alt.Reset();
		if (GEngine) GEngine->AddOnScreenDebugMessage(
			10, 3.f, FColor::Orange,
			TEXT(">> RTL ACTIVE <<"));
	}
	else
	{
		bRTLFollowingPath = false;
		bRTLHovering = false;
		RTLPath.Empty();
		DroneVelocity = FVector::ZeroVector;
		if (GEngine) GEngine->AddOnScreenDebugMessage(
			10, 2.f, FColor::White, TEXT(">> RTL OFF <<"));
	}
}

// ---------------------------------------------------------------------------
// Waypoint handlers — adds EXACT drone position including altitude
// ---------------------------------------------------------------------------

void ADronePawn::OnAddWaypoint()
{
	if (bWaypointMode) return;
	FVector WP = GetActorLocation(); // saves X, Y, Z at drone's current height
	Waypoints.Add(WP);
	if (GEngine) GEngine->AddOnScreenDebugMessage(
		13, 3.f, FColor::Cyan,
		FString::Printf(
			TEXT(">> WP[%d] SET  X:%.0f Y:%.0f Z:%.0f <<"),
			Waypoints.Num() - 1,
			WP.X, WP.Y, WP.Z));
}
void ADronePawn::OnStartWaypoint()
{
	if (Waypoints.Num() == 0)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(
			13, 2.f, FColor::Red,
			TEXT(">> NO WAYPOINTS! Fly to spots and press P <<"));
		return;
	}
	bWaypointMode = !bWaypointMode;
	if (bWaypointMode)
	{
		bLoiterMode = bRTLMode = false;
		CurrentWaypointIndex = 0;
		DroneVelocity = FVector::ZeroVector;
		Input_Throttle = Input_Pitch = Input_Roll = Input_Yaw = 0.f;
		PID_Alt.Reset();
		if (GEngine) GEngine->AddOnScreenDebugMessage(
			13, 3.f, FColor::Green,
			FString::Printf(
				TEXT(">> MISSION STARTED — %d WAYPOINTS <<"),
				Waypoints.Num()));
	}
	else
	{
		DroneVelocity = FVector::ZeroVector;
		if (GEngine) GEngine->AddOnScreenDebugMessage(
			13, 2.f, FColor::White,
			TEXT(">> MISSION STOPPED <<"));
	}
}
void ADronePawn::OnClearWaypoints()
{
	bWaypointMode = false;
	CurrentWaypointIndex = 0;
	Waypoints.Empty();
	DroneVelocity = FVector::ZeroVector;
	if (GEngine) GEngine->AddOnScreenDebugMessage(
		13, 2.f, FColor::Red,
		TEXT(">> WAYPOINTS CLEARED <<"));
}
void ADronePawn::OnToggleMissionPanel()
{
	if (DroneHUD) DroneHUD->ToggleMissionPanel();
}
void ADronePawn::OnMissionNextField()
{
	if (DroneHUD) DroneHUD->MissionNextField();
}
void ADronePawn::OnMissionAddWaypoint()
{
	if (DroneHUD) DroneHUD->MissionAddWaypoint();
}
void ADronePawn::OnMissionClearAll()
{
	bMissionMode = false;
	CurrentMissionIndex = 0;
	if (DroneHUD) DroneHUD->MissionClearAll();
}
void ADronePawn::OnStartMission()
{
	if (!DroneHUD) return;
	const TArray<FVector>& WPs = DroneHUD->GetMissionWaypoints();
	if (WPs.Num() == 0)
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(
			24, 3.f, FColor::Red,
			TEXT(">> NO WAYPOINTS! Press B to open panel <<"));
		return;
	}
	bMissionMode = !bMissionMode;
	if (bMissionMode)
	{
		bLoiterMode = false;
		bRTLMode = false;
		bWaypointMode = false;
		CurrentMissionIndex = 0;
		DroneVelocity = FVector::ZeroVector;
		Input_Throttle = Input_Pitch = Input_Roll = Input_Yaw = 0.f;
		PID_Alt.Reset();
		DroneHUD->SetMissionActive(true, 0);
		if (GEngine) GEngine->AddOnScreenDebugMessage(
			24, 3.f, FColor::Purple,
			FString::Printf(
				TEXT(">> MISSION STARTED — %d WPs <<"),
				WPs.Num()));
	}
	else
	{
		DroneVelocity = FVector::ZeroVector;
		DroneHUD->SetMissionActive(false, 0);
		if (GEngine) GEngine->AddOnScreenDebugMessage(
			24, 2.f, FColor::White,
			TEXT(">> MISSION STOPPED <<"));
	}
}