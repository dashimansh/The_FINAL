#include "DroneMapWidget.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Rendering/DrawElements.h"

void UDroneMapWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UDroneMapWidget::SetPiPRenderTarget(UTextureRenderTarget2D* InTarget)
{
	if (!InTarget) return;
	PiPBrush = FSlateBrush();
	PiPBrush.SetResourceObject(InTarget);
	PiPBrush.ImageSize = FVector2D(250.f, 180.f);
	PiPBrush.DrawAs = ESlateBrushDrawType::Image;
	bHasRenderTarget = true;
}

int32 UDroneMapWidget::NativePaint(
	const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
{
	int32 MaxLayer = Super::NativePaint(
		Args, AllottedGeometry, MyCullingRect,
		OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	FVector2f Size = (FVector2f)AllottedGeometry.GetLocalSize();

	// --- Dark background fallback ---
	FSlateBrush BG;
	BG.TintColor = FSlateColor(FLinearColor(0.05f, 0.05f, 0.05f, 0.95f));
	FSlateDrawElement::MakeBox(
		OutDrawElements, ++MaxLayer,
		AllottedGeometry.ToPaintGeometry(
			Size,
			FSlateLayoutTransform(FVector2f(0.f, 0.f))),
		&BG, ESlateDrawEffect::None,
		FLinearColor(0.05f, 0.05f, 0.05f, 0.95f));

	// --- PiP camera feed ---
	if (bHasRenderTarget)
	{
		FSlateDrawElement::MakeBox(
			OutDrawElements, ++MaxLayer,
			AllottedGeometry.ToPaintGeometry(
				Size,
				FSlateLayoutTransform(FVector2f(0.f, 0.f))),
			&PiPBrush,
			ESlateDrawEffect::None,
			FLinearColor::White);
	}

	// --- Border top ---
	FSlateBrush BB;
	BB.TintColor = FSlateColor(BorderColor);
	FSlateDrawElement::MakeBox(OutDrawElements, ++MaxLayer,
		AllottedGeometry.ToPaintGeometry(
			FVector2f(Size.X, 2.f),
			FSlateLayoutTransform(FVector2f(0.f, 0.f))),
		&BB, ESlateDrawEffect::None, BorderColor);

	// --- Border bottom ---
	FSlateDrawElement::MakeBox(OutDrawElements, ++MaxLayer,
		AllottedGeometry.ToPaintGeometry(
			FVector2f(Size.X, 2.f),
			FSlateLayoutTransform(FVector2f(0.f, Size.Y - 2.f))),
		&BB, ESlateDrawEffect::None, BorderColor);

	// --- Border left ---
	FSlateDrawElement::MakeBox(OutDrawElements, ++MaxLayer,
		AllottedGeometry.ToPaintGeometry(
			FVector2f(2.f, Size.Y),
			FSlateLayoutTransform(FVector2f(0.f, 0.f))),
		&BB, ESlateDrawEffect::None, BorderColor);

	// --- Border right ---
	FSlateDrawElement::MakeBox(OutDrawElements, ++MaxLayer,
		AllottedGeometry.ToPaintGeometry(
			FVector2f(2.f, Size.Y),
			FSlateLayoutTransform(FVector2f(Size.X - 2.f, 0.f))),
		&BB, ESlateDrawEffect::None, BorderColor);

	// --- Center crosshair dot ---
	float DotSize = 8.f;
	FVector2f Center = Size * 0.5f;
	FSlateBrush DB;
	DB.TintColor = FSlateColor(FLinearColor(0.f, 1.f, 0.5f, 0.9f));
	FSlateDrawElement::MakeBox(
		OutDrawElements, ++MaxLayer,
		AllottedGeometry.ToPaintGeometry(
			FVector2f(DotSize, DotSize),
			FSlateLayoutTransform(FVector2f(
				Center.X - DotSize * 0.5f,
				Center.Y - DotSize * 0.5f))),
		&DB, ESlateDrawEffect::None,
		FLinearColor(0.f, 1.f, 0.5f, 0.9f));

	// --- Label "PiP" top-left ---
	FSlateBrush LabelBG;
	LabelBG.TintColor = FSlateColor(
		FLinearColor(0.f, 0.f, 0.f, 0.6f));
	FSlateDrawElement::MakeBox(
		OutDrawElements, ++MaxLayer,
		AllottedGeometry.ToPaintGeometry(
			FVector2f(60.f, 18.f),
			FSlateLayoutTransform(FVector2f(4.f, 4.f))),
		&LabelBG, ESlateDrawEffect::None,
		FLinearColor(0.f, 0.f, 0.f, 0.6f));

	return MaxLayer;
}