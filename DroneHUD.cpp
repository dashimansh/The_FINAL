#include "DroneHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "GameFramework/PlayerController.h"

ADroneHUD::ADroneHUD() {}

void ADroneHUD::UpdateData(
	float InMotor_FL, float InMotor_FR,
	float InMotor_BL, float InMotor_BR,
	float InAltitude,
	float InThrottle, float InPitch,
	float InRoll, float InYaw,
	bool bInFPVMode,
	bool bInJoystickMode,
	bool bInBraking,
	float InPosX, float InPosY, float InPosZ,
	bool bInLoiterMode,
	bool bInRTLMode)
{
	Motor_FL = InMotor_FL;
	Motor_FR = InMotor_FR;
	Motor_BL = InMotor_BL;
	Motor_BR = InMotor_BR;
	Altitude = InAltitude;
	Throttle = InThrottle;
	Pitch = InPitch;
	Roll = InRoll;
	Yaw = InYaw;
	bFPVMode = bInFPVMode;
	bJoystickMode = bInJoystickMode;
	bBraking = bInBraking;
	PosX = InPosX;
	PosY = InPosY;
	PosZ = InPosZ;
	bLoiterMode = bInLoiterMode;
	bRTLMode = bInRTLMode;
}

void ADroneHUD::UpdateMinimapData(
	FVector InPos, FRotator InRot,
	float InGimbalYaw, bool bInGimbalOn)
{
	DroneWorldPos = InPos;
	DroneWorldRot = InRot;
	GimbalWorldYaw = InGimbalYaw;
	bGimbalOn = bInGimbalOn;
}

// ---------------------------------------------------------------------------
// Toggles
// ---------------------------------------------------------------------------

void ADroneHUD::ToggleHUD()
{
	bHUDVisible = !bHUDVisible;
	if (GEngine) GEngine->AddOnScreenDebugMessage(
		80, 2.f,
		bHUDVisible ? FColor::Green : FColor::Red,
		bHUDVisible ? TEXT(">> HUD ON <<") : TEXT(">> HUD OFF <<"));
}
void ADroneHUD::ToggleMotorPanel()
{
	bMotorPanelVisible = !bMotorPanelVisible;
}
void ADroneHUD::ToggleFlightPanel()
{
	bFlightPanelVisible = !bFlightPanelVisible;
}
void ADroneHUD::ToggleModePanel()
{
	bModePanelVisible = !bModePanelVisible;
}
void ADroneHUD::ToggleInputPanel()
{
	bInputPanelVisible = !bInputPanelVisible;
}
void ADroneHUD::ToggleMinimap()
{
	bMinimapVisible = !bMinimapVisible;
}
void ADroneHUD::TogglePiP()
{
	bPiPVisible = !bPiPVisible;
}
void ADroneHUD::ToggleRCDisplay()
{
	bRCDisplayVisible = !bRCDisplayVisible;
	if (GEngine) GEngine->AddOnScreenDebugMessage(
		97, 2.f,
		bRCDisplayVisible ? FColor::Green : FColor::Red,
		bRCDisplayVisible
		? TEXT(">> RC DISPLAY ON <<")
		: TEXT(">> RC DISPLAY OFF <<"));
}
void ADroneHUD::ToggleMissionPanel()
{
	bMissionPanelVisible = !bMissionPanelVisible;
	MissionInputBuffer = TEXT("");
	if (GEngine) GEngine->AddOnScreenDebugMessage(
		95, 2.f,
		bMissionPanelVisible ? FColor::Purple : FColor::White,
		bMissionPanelVisible
		? TEXT(">> MISSION PANEL ON (B) <<")
		: TEXT(">> MISSION PANEL OFF (B) <<"));
}
void ADroneHUD::MissionNextField()
{
	CommitMissionInput();
	MissionEditField = (MissionEditField + 1) % 3;
}
void ADroneHUD::CommitMissionInput()
{
	if (MissionInputBuffer.Len() == 0) return;
	float Val = FCString::Atof(*MissionInputBuffer);
	if (MissionEditField == 0) MissionEditX = Val;
	else if (MissionEditField == 1) MissionEditY = Val;
	else                            MissionEditZ = Val;
	MissionInputBuffer = TEXT("");
}
void ADroneHUD::MissionAddWaypoint()
{
	CommitMissionInput();
	MissionWaypointList.Add(
		FVector(MissionEditX, MissionEditY, MissionEditZ));
	if (GEngine) GEngine->AddOnScreenDebugMessage(
		96, 2.f, FColor::Purple,
		FString::Printf(
			TEXT(">> WP[%d] X:%.0f Y:%.0f Z:%.0f <<"),
			MissionWaypointList.Num() - 1,
			MissionEditX, MissionEditY, MissionEditZ));
}
void ADroneHUD::MissionClearAll()
{
	MissionWaypointList.Empty();
	bMissionActive = false;
	MissionCurrentWP = 0;
	MissionInputBuffer = TEXT("");
	MissionEditX = MissionEditY = 0.f;
	MissionEditZ = 500.f;
	MissionEditField = 0;
	if (GEngine) GEngine->AddOnScreenDebugMessage(
		96, 2.f, FColor::Red,
		TEXT(">> MISSION CLEARED <<"));
}
void ADroneHUD::SetMissionActive(bool bActive, int32 CurrentWP)
{
	bMissionActive = bActive;
	MissionCurrentWP = CurrentWP;
}

// ---------------------------------------------------------------------------
// Mission Typing Input
// ---------------------------------------------------------------------------

void ADroneHUD::HandleMissionInput()
{
	if (!bMissionPanelVisible) return;
	APlayerController* PC = GetOwningPlayerController();
	if (!PC) return;

	struct FKeyChar { FKey Key; FString Char; };

	TArray<FKeyChar> Digits = {
		{EKeys::Zero,  TEXT("0")}, {EKeys::One,   TEXT("1")},
		{EKeys::Two,   TEXT("2")}, {EKeys::Three, TEXT("3")},
		{EKeys::Four,  TEXT("4")}, {EKeys::Five,  TEXT("5")},
		{EKeys::Six,   TEXT("6")}, {EKeys::Seven, TEXT("7")},
		{EKeys::Eight, TEXT("8")}, {EKeys::Nine,  TEXT("9")},
	};
	for (auto& D : Digits)
		if (PC->WasInputKeyJustPressed(D.Key))
			MissionInputBuffer += D.Char;

	TArray<FKeyChar> Numpad = {
		{EKeys::NumPadZero,  TEXT("0")},
		{EKeys::NumPadOne,   TEXT("1")},
		{EKeys::NumPadTwo,   TEXT("2")},
		{EKeys::NumPadThree, TEXT("3")},
		{EKeys::NumPadFour,  TEXT("4")},
		{EKeys::NumPadFive,  TEXT("5")},
		{EKeys::NumPadSix,   TEXT("6")},
		{EKeys::NumPadSeven, TEXT("7")},
		{EKeys::NumPadEight, TEXT("8")},
		{EKeys::NumPadNine,  TEXT("9")},
	};
	for (auto& D : Numpad)
		if (PC->WasInputKeyJustPressed(D.Key))
			MissionInputBuffer += D.Char;

	if (PC->WasInputKeyJustPressed(EKeys::Hyphen))
	{
		if (MissionInputBuffer.StartsWith(TEXT("-")))
			MissionInputBuffer = MissionInputBuffer.RightChop(1);
		else
			MissionInputBuffer = TEXT("-") + MissionInputBuffer;
	}

	if (PC->WasInputKeyJustPressed(EKeys::BackSpace))
		if (MissionInputBuffer.Len() > 0)
			MissionInputBuffer = MissionInputBuffer.LeftChop(1);

	if (PC->WasInputKeyJustPressed(EKeys::Enter))
		CommitMissionInput();

	if (GetWorld())
	{
		MissionCursorTimer += GetWorld()->GetDeltaSeconds();
		if (MissionCursorTimer >= 0.5f)
		{
			MissionCursorTimer = 0.f;
			bMissionCursorVisible = !bMissionCursorVisible;
		}
	}
}

// ---------------------------------------------------------------------------
// DrawHUD
// ---------------------------------------------------------------------------

void ADroneHUD::DrawHUD()
{
	Super::DrawHUD();
	if (!Canvas || !bHUDVisible) return;

	if (bMotorPanelVisible)  DrawMotorPanel();
	if (bFlightPanelVisible) DrawFlightData();
	if (bModePanelVisible)   DrawModePanel();
	if (bInputPanelVisible)  DrawInputPanel();
	if (bMinimapVisible)     DrawMinimap();
	if (bPiPVisible)         DrawPiP();
	if (bGimbalViewVisible)  DrawGimbalView();
	if (bRCDisplayVisible)   DrawRCDisplay();
	if (bMissionPanelVisible)
	{
		HandleMissionInput();
		DrawMissionPanel();
	}
}

// ---------------------------------------------------------------------------
// Motor Panel
// ---------------------------------------------------------------------------

void ADroneHUD::DrawMotorPanel()
{
	float PX = 10.f, PY = 10.f;
	float PW = 320.f, PH = 170.f;
	DrawRect(ColorDark, PX, PY, PW, PH);
	DrawRect(FLinearColor(0.f, 1.f, 0.5f, 0.3f), PX, PY, PW, 2.f);
	DrawText(TEXT("-- MOTOR THRUST --"),
		ColorGreen, PX + 70.f, PY + 6.f,
		GEngine->GetSmallFont(), 1.1f);
	float BX = PX + 20.f, BY = PY + 30.f;
	float BW = 60.f, BH = 100.f, Gap = 70.f;
	DrawMotorBar(BX, BY, BW, BH, Motor_FL, TEXT("FL"));
	DrawMotorBar(BX + Gap, BY, BW, BH, Motor_FR, TEXT("FR"));
	DrawMotorBar(BX + Gap * 2.f, BY, BW, BH, Motor_BL, TEXT("BL"));
	DrawMotorBar(BX + Gap * 3.f, BY, BW, BH, Motor_BR, TEXT("BR"));
}

void ADroneHUD::DrawMotorBar(
	float X, float Y, float W, float H,
	float Value, const FString& Label)
{
	DrawRect(FLinearColor(0.1f, 0.1f, 0.1f, 1.f), X, Y, W, H);
	float FillH = H * FMath::Clamp(Value, 0.f, 1.f);
	FLinearColor BC = Value > 0.7f ? ColorRed
		: Value > 0.3f ? ColorAmber : ColorGreen;
	DrawRect(BC, X, Y + H - FillH, W, FillH);
	DrawRect(ColorGray, X, Y, W, 1.f);
	DrawRect(ColorGray, X, Y + H, W, 1.f);
	DrawRect(ColorGray, X, Y, 1.f, H);
	DrawRect(ColorGray, X + W, Y, 1.f, H);
	DrawText(FString::Printf(TEXT("%.0f%%"), Value * 100.f),
		ColorWhite, X + 4.f, Y + H + 4.f,
		GEngine->GetSmallFont(), 0.9f);
	DrawText(Label, ColorGreen, X + 4.f, Y - 14.f,
		GEngine->GetSmallFont(), 1.f);
}

// ---------------------------------------------------------------------------
// Flight Data Panel
// ---------------------------------------------------------------------------

void ADroneHUD::DrawFlightData()
{
	float PW = 220.f, PH = 140.f;
	float PX = Canvas->SizeX * 0.5f - PW * 0.5f;
	float PY = 10.f;
	DrawRect(ColorDark, PX, PY, PW, PH);
	DrawRect(FLinearColor(0.f, 1.f, 0.5f, 0.3f), PX, PY, PW, 2.f);
	DrawText(TEXT("-- FLIGHT DATA --"),
		ColorGreen, PX + 40.f, PY + 6.f,
		GEngine->GetSmallFont(), 1.1f);
	float TX = PX + 10.f, TY = PY + 24.f, LS = 16.f;
	DrawText(FString::Printf(TEXT("ALT  : %.2f m"), Altitude),
		ColorCyan, TX, TY, GEngine->GetSmallFont(), 1.f);
	DrawText(FString::Printf(TEXT("PITCH: %.1f"), Pitch),
		ColorWhite, TX, TY + LS, GEngine->GetSmallFont(), 1.f);
	DrawText(FString::Printf(TEXT("ROLL : %.1f"), Roll),
		ColorWhite, TX, TY + LS * 2, GEngine->GetSmallFont(), 1.f);
	DrawRect(ColorGray, PX + 8.f, TY + LS * 3 - 2.f, PW - 16.f, 1.f);
	DrawText(FString::Printf(TEXT("X: %.1f m"), PosX / 100.f),
		ColorGreen, TX, TY + LS * 3, GEngine->GetSmallFont(), 1.f);
	DrawText(FString::Printf(TEXT("Y: %.1f m"), PosY / 100.f),
		ColorGreen, TX, TY + LS * 4, GEngine->GetSmallFont(), 1.f);
	DrawText(FString::Printf(TEXT("Z: %.1f m"), PosZ / 100.f),
		ColorGreen, TX, TY + LS * 5, GEngine->GetSmallFont(), 1.f);
}

// ---------------------------------------------------------------------------
// Mode Panel
// ---------------------------------------------------------------------------

void ADroneHUD::DrawModePanel()
{
	float PW = 220.f, PH = 295.f;
	float PX = Canvas->SizeX - PW - 10.f;
	float PY = 10.f;
	DrawRect(ColorDark, PX, PY, PW, PH);
	DrawRect(FLinearColor(0.f, 1.f, 0.5f, 0.3f), PX, PY, PW, 2.f);
	float TX = PX + 10.f, TY = PY + 8.f, LS = 16.f;
	DrawText(bFPVMode ? TEXT("CAM : FPV") : TEXT("CAM : 3RD"),
		ColorAmber, TX, TY, GEngine->GetSmallFont(), 1.f); TY += LS;
	DrawText(bJoystickMode
		? TEXT("INPUT: JOYSTICK") : TEXT("INPUT: KEYBOARD"),
		ColorWhite, TX, TY, GEngine->GetSmallFont(), 1.f); TY += LS;
	DrawText(bBraking ? TEXT("BRAKE: ON") : TEXT("BRAKE: OFF"),
		bBraking ? ColorRed : ColorGray,
		TX, TY, GEngine->GetSmallFont(), 1.f); TY += LS;
	DrawText(bLoiterMode ? TEXT("LOITER: ON") : TEXT("LOITER: OFF"),
		bLoiterMode ? ColorCyan : ColorGray,
		TX, TY, GEngine->GetSmallFont(), 1.f); TY += LS;
	DrawText(bRTLMode ? TEXT("RTL: ON") : TEXT("RTL: OFF"),
		bRTLMode ? ColorOrange : ColorGray,
		TX, TY, GEngine->GetSmallFont(), 1.f); TY += LS + 4.f;
	DrawRect(ColorGray, TX, TY, PW - 20.f, 1.f); TY += 6.f;
	DrawText(TEXT("-- KEYS --"), ColorGreen, TX, TY,
		GEngine->GetSmallFont(), 1.f); TY += LS;
	const TArray<FString> Keys = {
		TEXT("W/S  = Forward/Back"),
		TEXT("A/D  = Left/Right"),
		TEXT("E/Q  = Up/Down"),
		TEXT("Z/X  = Yaw"),
		TEXT("SPACE= Brake"),
		TEXT("P=WP  O=Fly  K=Clear"),
		TEXT("G=Gimbal(Scroll=Zoom)"),
		TEXT("H=HUD  M=Map  B=Mission"),
		TEXT("1=Motor 2=Flight"),
		TEXT("3=Mode  4=Input"),
		TEXT("N=ArmLights V=Body"),
		TEXT("Y=Trail F=Clear"),
		TEXT("L=Loiter R=RTL"),
		TEXT("C=Cam  T=InputMode"),
	};
	for (const FString& K : Keys)
	{
		DrawText(K, ColorGray, TX, TY,
			GEngine->GetSmallFont(), 0.9f);
		TY += 14.f;
	}
}

// ---------------------------------------------------------------------------
// Input Panel
// ---------------------------------------------------------------------------

void ADroneHUD::DrawInputPanel()
{
	float PX = 10.f, PY = 190.f;
	float PW = 320.f, PH = 110.f;
	DrawRect(ColorDark, PX, PY, PW, PH);
	DrawRect(FLinearColor(0.f, 1.f, 0.5f, 0.3f), PX, PY, PW, 2.f);
	DrawText(TEXT("-- INPUT --"),
		ColorGreen, PX + 110.f, PY + 6.f,
		GEngine->GetSmallFont(), 1.1f);
	float BX = PX + 10.f, BW = PW - 80.f;
	float BY = PY + 26.f, LS = 20.f;
	DrawInputBar(BX, BY, BW, Throttle, TEXT("THROT"));
	DrawInputBar(BX, BY + LS, BW, Pitch, TEXT("PITCH"));
	DrawInputBar(BX, BY + LS * 2, BW, Roll, TEXT("ROLL "));
	DrawInputBar(BX, BY + LS * 3, BW, Yaw, TEXT("YAW  "));
}

void ADroneHUD::DrawInputBar(
	float X, float Y, float W,
	float Value, const FString& Label)
{
	DrawText(Label, ColorGray, X, Y,
		GEngine->GetSmallFont(), 0.9f);
	float BX = X + 55.f, BW = W - 55.f, BH = 10.f;
	DrawRect(FLinearColor(0.1f, 0.1f, 0.1f, 1.f), BX, Y, BW, BH);
	float Mid = BX + BW * 0.5f, Fill = Value * BW * 0.5f;
	if (Fill > 0.f)      DrawRect(ColorGreen, Mid, Y, Fill, BH);
	else if (Fill < 0.f) DrawRect(ColorRed, Mid + Fill, Y, -Fill, BH);
	DrawRect(ColorGray, Mid, Y, 1.f, BH);
	DrawText(FString::Printf(TEXT("%.2f"), Value),
		ColorWhite, BX + BW + 4.f, Y,
		GEngine->GetSmallFont(), 0.9f);
}

// ---------------------------------------------------------------------------
// Minimap
// ---------------------------------------------------------------------------

void ADroneHUD::DrawMinimap()
{
	float MS = MinimapSize;
	float PX = Canvas->SizeX - MS - 10.f;
	float PY = Canvas->SizeY - MS - 10.f;
	float CX = PX + MS * 0.5f, CY = PY + MS * 0.5f;
	float R = MS * 0.5f - 4.f;
	DrawRect(ColorDark, PX, PY, MS, MS);
	int32 Segs = 64;
	for (int32 i = 0; i < Segs; i++)
	{
		float A1 = FMath::DegreesToRadians(360.f / Segs * i);
		float A2 = FMath::DegreesToRadians(360.f / Segs * (i + 1));
		DrawLine(CX + FMath::Cos(A1) * R, CY + FMath::Sin(A1) * R,
			CX + FMath::Cos(A2) * R, CY + FMath::Sin(A2) * R,
			ColorGreen, 1.5f);
	}
	DrawText(TEXT("N"), ColorGreen, CX - 4.f, PY + 2.f,
		GEngine->GetSmallFont(), 1.f);
	DrawText(TEXT("S"), ColorGreen, CX - 4.f, PY + MS - 14.f,
		GEngine->GetSmallFont(), 1.f);
	DrawText(TEXT("W"), ColorGreen, PX + 2.f, CY - 6.f,
		GEngine->GetSmallFont(), 1.f);
	DrawText(TEXT("E"), ColorGreen, PX + MS - 12.f, CY - 6.f,
		GEngine->GetSmallFont(), 1.f);
	DrawText(FString::Printf(TEXT("%.0fm"), MinimapRange / 100.f),
		ColorGray, PX + MS - 36.f, PY + MS - 14.f,
		GEngine->GetSmallFont(), 0.85f);
	DrawText(TEXT("MAP"), ColorGreen, PX + 6.f, PY + 4.f,
		GEngine->GetSmallFont(), 0.9f);
	DrawMinimapArrow(CX, CY, DroneWorldRot.Yaw, 12.f, ColorGreen);
	if (bGimbalOn)
	{
		float GR = FMath::DegreesToRadians(GimbalWorldYaw);
		float CS = 20.f, CSp = 20.f;
		float A1 = GR - FMath::DegreesToRadians(CSp);
		float A2 = GR + FMath::DegreesToRadians(CSp);
		DrawLine(CX, CY,
			CX + FMath::Cos(A1) * CS, CY + FMath::Sin(A1) * CS,
			FLinearColor(0.f, 0.8f, 1.f, 0.6f), 1.5f);
		DrawLine(CX, CY,
			CX + FMath::Cos(A2) * CS, CY + FMath::Sin(A2) * CS,
			FLinearColor(0.f, 0.8f, 1.f, 0.6f), 1.5f);
	}
}

void ADroneHUD::DrawMinimapArrow(
	float CX, float CY, float Angle,
	float Size, FLinearColor Color)
{
	float Rad = FMath::DegreesToRadians(Angle - 90.f);
	float TipX = CX + FMath::Cos(Rad) * Size;
	float TipY = CY + FMath::Sin(Rad) * Size;
	float L1R = Rad + FMath::DegreesToRadians(140.f);
	float L2R = Rad - FMath::DegreesToRadians(140.f);
	float L1X = CX + FMath::Cos(L1R) * Size * 0.6f;
	float L1Y = CY + FMath::Sin(L1R) * Size * 0.6f;
	float L2X = CX + FMath::Cos(L2R) * Size * 0.6f;
	float L2Y = CY + FMath::Sin(L2R) * Size * 0.6f;
	DrawLine(TipX, TipY, L1X, L1Y, Color, 2.f);
	DrawLine(TipX, TipY, L2X, L2Y, Color, 2.f);
	DrawLine(L1X, L1Y, L2X, L2Y, Color, 2.f);
}

// ---------------------------------------------------------------------------
// PiP — bottom right
// ---------------------------------------------------------------------------

void ADroneHUD::DrawPiP()
{
	float PX = Canvas->SizeX - PiPWidth - 10.f;
	float PY = Canvas->SizeY - MinimapSize - PiPHeight - 20.f;
	float CX = PX + PiPWidth * 0.5f, CY = PY + PiPHeight * 0.5f;
	DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.95f),
		PX, PY, PiPWidth, PiPHeight);
	if (PiPTarget)
		Canvas->K2_DrawTexture(PiPTarget,
			FVector2D(PX, PY), FVector2D(PiPWidth, PiPHeight),
			FVector2D(0.f, 0.f), FVector2D(1.f, 1.f),
			FLinearColor::White, BLEND_Translucent);
	DrawRect(ColorGreen, PX, PY, PiPWidth, 2.f);
	DrawRect(ColorGreen, PX, PY + PiPHeight - 2.f, PiPWidth, 2.f);
	DrawRect(ColorGreen, PX, PY, 2.f, PiPHeight);
	DrawRect(ColorGreen, PX + PiPWidth - 2.f, PY, 2.f, PiPHeight);
	DrawRect(FLinearColor(0.f, 1.f, 0.5f, 0.4f),
		CX - 20.f, CY - 0.5f, 40.f, 1.f);
	DrawRect(FLinearColor(0.f, 1.f, 0.5f, 0.4f),
		CX - 0.5f, CY - 20.f, 1.f, 40.f);
	float DS = 10.f;
	DrawRect(FLinearColor(0.f, 1.f, 0.5f, 1.f),
		CX - DS * 0.5f, CY - DS * 0.5f, DS, DS);
	int32 Segs = 20; float Radius = 18.f;
	for (int32 i = 0; i < Segs; i++)
	{
		float A1 = FMath::DegreesToRadians((360.f / Segs) * i);
		float A2 = FMath::DegreesToRadians((360.f / Segs) * (i + 1));
		DrawLine(CX + FMath::Cos(A1) * Radius,
			CY + FMath::Sin(A1) * Radius,
			CX + FMath::Cos(A2) * Radius,
			CY + FMath::Sin(A2) * Radius,
			FLinearColor(0.f, 1.f, 0.5f, 0.9f), 1.5f);
	}
	DrawText(TEXT("CAM"), ColorGreen, PX + 6.f, PY + 5.f,
		GEngine->GetSmallFont(), 1.f);
	DrawText(FString::Printf(TEXT("ALT: %.1fm"), Altitude),
		ColorGreen, PX + 6.f, PY + PiPHeight - 16.f,
		GEngine->GetSmallFont(), 0.9f);
}

// ---------------------------------------------------------------------------
// Gimbal View — bottom left
// ---------------------------------------------------------------------------

void ADroneHUD::DrawGimbalView()
{
	if (!Canvas || !GimbalTarget) return;
	float PX = 10.f;
	float PY = Canvas->SizeY - GimbalViewHeight - 10.f;
	float CX = PX + GimbalViewWidth * 0.5f;
	float CY = PY + GimbalViewHeight * 0.5f;
	DrawRect(FLinearColor(0.f, 0.f, 0.f, 0.95f),
		PX, PY, GimbalViewWidth, GimbalViewHeight);
	Canvas->K2_DrawTexture(GimbalTarget,
		FVector2D(PX, PY),
		FVector2D(GimbalViewWidth, GimbalViewHeight),
		FVector2D(0.f, 0.f), FVector2D(1.f, 1.f),
		FLinearColor::White, BLEND_Translucent);
	FLinearColor BC = bGimbalOn
		? FLinearColor(0.f, 1.f, 1.f, 1.f)
		: FLinearColor(0.3f, 0.3f, 0.3f, 1.f);
	DrawRect(BC, PX, PY, GimbalViewWidth, 2.f);
	DrawRect(BC, PX, PY + GimbalViewHeight - 2.f, GimbalViewWidth, 2.f);
	DrawRect(BC, PX, PY, 2.f, GimbalViewHeight);
	DrawRect(BC, PX + GimbalViewWidth - 2.f, PY, 2.f, GimbalViewHeight);
	DrawRect(FLinearColor(0.f, 1.f, 1.f, 0.4f),
		CX - 20.f, CY - 0.5f, 40.f, 1.f);
	DrawRect(FLinearColor(0.f, 1.f, 1.f, 0.4f),
		CX - 0.5f, CY - 20.f, 1.f, 40.f);
	float DS = 10.f;
	DrawRect(bGimbalOn
		? FLinearColor(0.f, 1.f, 1.f, 1.f)
		: FLinearColor(0.3f, 0.3f, 0.3f, 1.f),
		CX - DS * 0.5f, CY - DS * 0.5f, DS, DS);
	int32 Segs = 20; float Radius = 18.f;
	for (int32 i = 0; i < Segs; i++)
	{
		float A1 = FMath::DegreesToRadians((360.f / Segs) * i);
		float A2 = FMath::DegreesToRadians((360.f / Segs) * (i + 1));
		DrawLine(CX + FMath::Cos(A1) * Radius,
			CY + FMath::Sin(A1) * Radius,
			CX + FMath::Cos(A2) * Radius,
			CY + FMath::Sin(A2) * Radius,
			FLinearColor(0.f, 1.f, 1.f, 0.9f), 1.5f);
	}
	DrawText(bGimbalOn ? TEXT("GIMBAL CAM") : TEXT("GIMBAL CAM [OFF]"),
		bGimbalOn ? ColorCyan : ColorGray,
		PX + 6.f, PY + 5.f, GEngine->GetSmallFont(), 1.f);
	DrawText(FString::Printf(TEXT("PITCH: %.0f"), GimbalWorldYaw),
		ColorGray, PX + 6.f, PY + GimbalViewHeight - 16.f,
		GEngine->GetSmallFont(), 0.9f);
}

// ---------------------------------------------------------------------------
// RC Controller Display — center bottom
// ---------------------------------------------------------------------------

void ADroneHUD::DrawCircle(
	float CX, float CY, float Radius,
	FLinearColor Color, float Thickness, int32 Segments)
{
	for (int32 i = 0; i < Segments; i++)
	{
		float A1 = FMath::DegreesToRadians((360.f / Segments) * i);
		float A2 = FMath::DegreesToRadians((360.f / Segments) * (i + 1));
		DrawLine(CX + FMath::Cos(A1) * Radius,
			CY + FMath::Sin(A1) * Radius,
			CX + FMath::Cos(A2) * Radius,
			CY + FMath::Sin(A2) * Radius,
			Color, Thickness);
	}
}

void ADroneHUD::DrawStick(
	float CX, float CY, float Radius,
	float StickX, float StickY,
	const FString& LabelTop,
	const FString& LabelLeft,
	const FString& LabelRight,
	const FString& LabelBottom)
{
	// Background fill
	DrawRect(FLinearColor(0.1f, 0.1f, 0.1f, 0.9f),
		CX - Radius, CY - Radius, Radius * 2.f, Radius * 2.f);

	// Outer ring
	DrawCircle(CX, CY, Radius,
		FLinearColor(0.3f, 0.3f, 0.3f, 1.f), 2.f, 48);

	// Cross hairs
	DrawLine(CX - Radius, CY, CX + Radius, CY,
		FLinearColor(0.25f, 0.25f, 0.25f, 1.f), 1.f);
	DrawLine(CX, CY - Radius, CX, CY + Radius,
		FLinearColor(0.25f, 0.25f, 0.25f, 1.f), 1.f);

	// Inner guide
	DrawCircle(CX, CY, Radius * 0.5f,
		FLinearColor(0.2f, 0.2f, 0.2f, 1.f), 1.f, 32);

	// Stick dot position
	float SX = CX + StickX * Radius * 0.9f;
	float SY = CY - StickY * Radius * 0.9f;

	// Trail line
	DrawLine(CX, CY, SX, SY,
		FLinearColor(0.f, 1.f, 0.5f, 0.3f), 2.f);

	// Glow ring
	DrawCircle(SX, SY, 12.f,
		FLinearColor(0.f, 1.f, 0.5f, 0.3f), 4.f, 16);

	// Dot
	DrawCircle(SX, SY, 7.f,
		FLinearColor(0.f, 1.f, 0.5f, 1.f), 3.f, 16);

	// Center mark
	DrawRect(FLinearColor(0.4f, 0.4f, 0.4f, 1.f),
		CX - 2.f, CY - 2.f, 4.f, 4.f);

	// Labels
	DrawText(LabelTop,
		ColorGray, CX - 16.f, CY - Radius - 18.f,
		GEngine->GetSmallFont(), 0.85f);
	DrawText(LabelBottom,
		ColorGray, CX - 20.f, CY + Radius + 4.f,
		GEngine->GetSmallFont(), 0.85f);
	DrawText(LabelLeft,
		ColorGray, CX - Radius - 28.f, CY - 6.f,
		GEngine->GetSmallFont(), 0.85f);
	DrawText(LabelRight,
		ColorGray, CX + Radius + 4.f, CY - 6.f,
		GEngine->GetSmallFont(), 0.85f);
}

void ADroneHUD::DrawRCDisplay()
{
	if (!Canvas) return;

	float R = RCStickRadius;
	float Spacing = 80.f;
	float TotalW = R * 2.f * 2.f + Spacing + 80.f;
	float BaseX = Canvas->SizeX * 0.5f - TotalW * 0.5f;
	float BaseY = Canvas->SizeY - R - 30.f;

	float PW = TotalW + 40.f;
	float PH = R * 2.f + 50.f;
	float PX = BaseX - 20.f;
	float PY = BaseY - R - 20.f;

	DrawRect(FLinearColor(0.03f, 0.03f, 0.05f, 0.88f),
		PX, PY, PW, PH);
	DrawRect(FLinearColor(0.f, 1.f, 0.5f, 0.4f),
		PX, PY, PW, 2.f);
	DrawText(TEXT("RC CONTROLLER"),
		FLinearColor(0.f, 1.f, 0.5f, 0.6f),
		PX + PW * 0.5f - 50.f, PY + 4.f,
		GEngine->GetSmallFont(), 0.85f);

	// LEFT STICK — Throttle/Yaw
	float LCX = BaseX + R;
	float LCY = BaseY;
	DrawStick(LCX, LCY, R,
		Yaw, Throttle,
		TEXT("THR+"), TEXT("YAW-"), TEXT("YAW+"), TEXT("THR-"));
	DrawText(TEXT("LEFT STICK"),
		FLinearColor(0.6f, 0.6f, 0.6f, 1.f),
		LCX - 28.f, PY + PH - 14.f,
		GEngine->GetSmallFont(), 0.8f);

	// RIGHT STICK — Pitch/Roll
	float RCX = BaseX + R * 2.f + Spacing + 40.f;
	float RCY = BaseY;
	DrawStick(RCX, RCY, R,
		Roll, Pitch,
		TEXT("FWD"), TEXT("LEFT"), TEXT("RIGHT"), TEXT("BACK"));
	DrawText(TEXT("RIGHT STICK"),
		FLinearColor(0.6f, 0.6f, 0.6f, 1.f),
		RCX - 30.f, PY + PH - 14.f,
		GEngine->GetSmallFont(), 0.8f);

	// Center status
	float MX = (LCX + RCX) * 0.5f;
	float MY = BaseY - 20.f;
	float LS2 = 14.f;

	if (bLoiterMode)
		DrawText(TEXT("LOITER"), ColorCyan,
			MX - 20.f, MY, GEngine->GetSmallFont(), 0.9f);
	if (bRTLMode)
		DrawText(TEXT("RTL"), ColorOrange,
			MX - 10.f, MY + LS2, GEngine->GetSmallFont(), 0.9f);
	if (bBraking)
		DrawText(TEXT("BRAKE"), ColorRed,
			MX - 15.f, MY + LS2 * 2.f, GEngine->GetSmallFont(), 0.9f);

	DrawText(FString::Printf(TEXT("ALT %.1fm"), Altitude),
		ColorGreen, MX - 22.f, MY + LS2 * 3.f,
		GEngine->GetSmallFont(), 0.9f);
}

// ---------------------------------------------------------------------------
// Mission Panel
// ---------------------------------------------------------------------------

void ADroneHUD::DrawMissionPanel()
{
	if (!Canvas) return;

	float PW = 320.f;
	float PH = 420.f + MissionWaypointList.Num() * 18.f;
	float PX = 10.f, PY = 310.f;

	DrawRect(ColorDark, PX, PY, PW, PH);
	DrawRect(ColorPurple, PX, PY, PW, 2.f);

	float TX = PX + 10.f, TY = PY + 8.f, LS = 18.f;

	DrawText(TEXT("-- MISSION PLANNER --"),
		ColorPurple, TX + 30.f, TY,
		GEngine->GetSmallFont(), 1.1f);
	TY += LS + 4.f;

	DrawRect(ColorGray, TX, TY, PW - 20.f, 1.f);
	TY += 6.f;

	auto GetDisplay = [&](int32 FieldIdx, float StoredVal) -> FString
		{
			if (MissionEditField == FieldIdx)
			{
				FString D = MissionInputBuffer.Len() > 0
					? MissionInputBuffer
					: FString::Printf(TEXT("%.0f"), StoredVal);
				if (bMissionCursorVisible) D += TEXT("_");
				return D;
			}
			return FString::Printf(TEXT("%.0f"), StoredVal);
		};

	DrawText(FString::Printf(TEXT("%sX : %s"),
		MissionEditField == 0 ? TEXT("► ") : TEXT("  "),
		*GetDisplay(0, MissionEditX)),
		MissionEditField == 0 ? ColorYellow : ColorGray,
		TX, TY, GEngine->GetSmallFont(), 1.1f); TY += LS;

	DrawText(FString::Printf(TEXT("%sY : %s"),
		MissionEditField == 1 ? TEXT("► ") : TEXT("  "),
		*GetDisplay(1, MissionEditY)),
		MissionEditField == 1 ? ColorYellow : ColorGray,
		TX, TY, GEngine->GetSmallFont(), 1.1f); TY += LS;

	DrawText(FString::Printf(TEXT("%sZ : %s"),
		MissionEditField == 2 ? TEXT("► ") : TEXT("  "),
		*GetDisplay(2, MissionEditZ)),
		MissionEditField == 2 ? ColorYellow : ColorGray,
		TX, TY, GEngine->GetSmallFont(), 1.1f); TY += LS + 4.f;

	DrawRect(ColorGray, TX, TY, PW - 20.f, 1.f); TY += 6.f;

	DrawText(TEXT("TAB   = Confirm + next field"),
		ColorGray, TX, TY, GEngine->GetSmallFont(), 0.9f); TY += 14.f;
	DrawText(TEXT("ENTER = Confirm value"),
		ColorGray, TX, TY, GEngine->GetSmallFont(), 0.9f); TY += 14.f;
	DrawText(TEXT("BKSP  = Delete last digit"),
		ColorGray, TX, TY, GEngine->GetSmallFont(), 0.9f); TY += 14.f;
	DrawText(TEXT("-     = Toggle negative"),
		ColorGray, TX, TY, GEngine->GetSmallFont(), 0.9f); TY += 14.f;
	DrawText(TEXT("I     = Add Waypoint"),
		ColorGreen, TX, TY, GEngine->GetSmallFont(), 0.9f); TY += 14.f;
	DrawText(TEXT("DEL   = Clear All"),
		ColorRed, TX, TY, GEngine->GetSmallFont(), 0.9f); TY += 14.f;
	DrawText(TEXT("U     = Start / Stop Mission"),
		ColorPurple, TX, TY, GEngine->GetSmallFont(), 0.9f); TY += LS + 4.f;

	DrawRect(ColorGray, TX, TY, PW - 20.f, 1.f); TY += 6.f;

	DrawText(bMissionActive
		? FString::Printf(TEXT("FLYING TO WP[%d] / %d"),
			MissionCurrentWP, MissionWaypointList.Num())
		: (MissionWaypointList.Num() > 0
			? TEXT("READY — Press U to fly")
			: TEXT("Type X,Y,Z then press I to add")),
		bMissionActive ? ColorPurple : ColorGray,
		TX, TY, GEngine->GetSmallFont(), 0.9f);
	TY += LS + 4.f;

	DrawText(FString::Printf(TEXT("-- WAYPOINTS (%d) --"),
		MissionWaypointList.Num()),
		ColorGreen, TX, TY, GEngine->GetSmallFont(), 1.f);
	TY += LS;

	for (int32 i = 0; i < MissionWaypointList.Num(); i++)
	{
		FLinearColor WPC;
		FString Prefix;
		if (bMissionActive && i < MissionCurrentWP)
		{
			WPC = ColorGreen; Prefix = TEXT("OK ");
		}
		else if (bMissionActive && i == MissionCurrentWP)
		{
			WPC = ColorYellow; Prefix = TEXT(">> ");
		}
		else
		{
			WPC = ColorPurple; Prefix = TEXT("   ");
		}
		DrawText(FString::Printf(
			TEXT("%sWP[%d] X:%.0f Y:%.0f Z:%.0f"),
			*Prefix, i,
			MissionWaypointList[i].X,
			MissionWaypointList[i].Y,
			MissionWaypointList[i].Z),
			WPC, TX, TY, GEngine->GetSmallFont(), 0.9f);
		TY += 16.f;
	}
}