#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Styling/SlateBrush.h"
#include "DroneMapWidget.generated.h"

UCLASS()
class DRONE_PACK_API UDroneMapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "PiP")
	void SetPiPRenderTarget(UTextureRenderTarget2D* InTarget);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PiP")
	FLinearColor BorderColor = FLinearColor(0.f, 1.f, 0.5f, 0.9f);

protected:
	virtual void NativeConstruct() override;

	virtual int32 NativePaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;

private:
	FSlateBrush PiPBrush;
	bool bHasRenderTarget = false;
};