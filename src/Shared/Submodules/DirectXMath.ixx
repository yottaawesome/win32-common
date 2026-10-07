module;

#include <DirectXMath.h>
#include <DirectXCollision.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>

export module DirectXMath;

export
{
	using
		::DirectX::operator*,
		::DirectX::operator*=,
		::DirectX::operator-,
		::DirectX::operator-=,
		::DirectX::operator+,
		::DirectX::operator+=,
		::DirectX::operator/=,
		::DirectX::operator/
		;
}

export namespace DirectX
{
	constexpr auto Pi = DirectX::XM_PI;
	constexpr auto TwoPi = DirectX::XM_2PI;
	constexpr auto PiOver2 = DirectX::XM_PIDIV2;
	constexpr auto PiOver4 = DirectX::XM_PIDIV4;

	using
		::DirectX::XMFLOAT4,
		::DirectX::XMMATRIX,
		::DirectX::CXMMATRIX,
		::DirectX::FXMMATRIX,
		::DirectX::FXMVECTOR,
		::DirectX::CXMVECTOR,
		::DirectX::XMVECTORF32,
		::DirectX::XMFLOAT3,
		::DirectX::XMFLOAT2,
		::DirectX::XMFLOAT4X4,
		::DirectX::XMVECTOR,
		::DirectX::BoundingBox, // replaces XNA::AxisAlignedBox
		::DirectX::BoundingFrustum, // replaces XNA::Frustum
		::DirectX::XMVectorSet,
		::DirectX::XMVectorSubtract,
		::DirectX::XMVectorLerp,
		::DirectX::XMVectorGetX,
		::DirectX::XMVectorMultiplyAdd,
		::DirectX::XMVectorReplicate,
		::DirectX::XMVectorSwizzle,
		::DirectX::XMVectorScale,
		::DirectX::XMVectorMin,
		::DirectX::XMVectorMax,
		::DirectX::XMVectorReplicatePtr,
		::DirectX::XMVectorSqrt,
		::DirectX::XMVectorAdd,
		::DirectX::XMVectorZero,
		::DirectX::XMVectorSet,
		::DirectX::XMVectorFalseInt,
		::DirectX::XMVectorAndCInt,
		::DirectX::XMVectorSqrt,
		::DirectX::XMVectorLessOrEqual,
		::DirectX::XMVectorSplatW,
		::DirectX::XMVectorAndInt,
		::DirectX::XMVectorLess,
		::DirectX::XMVectorReciprocal,
		::DirectX::XMVectorGreater,
		::DirectX::XMVectorSelect,
		::DirectX::XMVectorEqual,
		::DirectX::XMVectorTrueInt,
		::DirectX::XMVectorOrInt,
		::DirectX::XMVectorInBounds,
		::DirectX::XMVectorSplatEpsilon,
		::DirectX::XMVectorGreaterOrEqual,
		::DirectX::XMVector3NearEqual,
		::DirectX::XMVector3Equal,
		::DirectX::XMVectorGetW,
		::DirectX::XMVectorSetW,
		::DirectX::XMVector3NotEqual,
		::DirectX::XMVector3LessOrEqual,
		::DirectX::XMVector3GreaterOrEqual,
		::DirectX::XMVector3EqualInt,
		::DirectX::XMVector3Transform,
		::DirectX::XMVector3Length,
		::DirectX::XMVector3Greater,
		::DirectX::XMVector3Normalize,
		::DirectX::XMVector3Dot,
		::DirectX::XMVector3Cross,
		::DirectX::XMVector3LengthSq,
		::DirectX::XMVector3TransformNormal,
		::DirectX::XMVector3Less,
		::DirectX::XMVector3TransformCoord,
		::DirectX::XMVector4Normalize,
		::DirectX::XMVector4EqualInt,
		::DirectX::XMVector4Reflect,
		::DirectX::XMVector4Dot,
		::DirectX::XMMatrixDecompose,
		::DirectX::XMMatrixTranslationFromVector,
		::DirectX::XMMatrixScalingFromVector,
		::DirectX::XMMatrixOrthographicOffCenterLH,
		::DirectX::XMMatrixAffineTransformation,
		::DirectX::XMMatrixRotationRollPitchYawFromVector,
		::DirectX::XMMatrixIdentity,
		::DirectX::XMMatrixMultiply,
		::DirectX::XMMatrixLookAtLH,
		::DirectX::XMMatrixSet,
		::DirectX::XMMatrixLookToLH,
		::DirectX::XMMatrixRotationAxis,
		::DirectX::XMMatrixTranslation,
		::DirectX::XMMatrixPerspectiveFovLH,
		::DirectX::XMMatrixScaling,
		::DirectX::XMMatrixDeterminant,
		::DirectX::XMMatrixRotationRollPitchYaw,
		::DirectX::XMMatrixTranspose,
		::DirectX::XMMatrixRotationQuaternion,
		::DirectX::XMMatrixShadow,
		::DirectX::XMMatrixRotationY,
		::DirectX::XMMatrixReflect,
		::DirectX::XMMatrixRotationX,
		::DirectX::XMMatrixRotationY,
		::DirectX::XMMatrixRotationZ,
		::DirectX::XMMatrixInverse,
		::DirectX::XMLoadFloat,
		::DirectX::XMStoreFloat,
		::DirectX::XMLoadFloat2,
		::DirectX::XMStoreFloat2,
		::DirectX::XMLoadFloat4,
		::DirectX::XMStoreFloat4,
		::DirectX::XMLoadFloat3,
		::DirectX::XMStoreFloat3,
		::DirectX::XMLoadFloat4x4,
		::DirectX::XMLoadFloat4x4,
		::DirectX::XMStoreFloat4x4,
		::DirectX::XMConvertToRadians,
		::DirectX::XMConvertToDegrees,
		::DirectX::XMConvertVectorFloatToInt,
		::DirectX::XMConvertVectorIntToFloat,
		::DirectX::XMPlaneFromPoints,
		::DirectX::XMPlaneNormalize,
		::DirectX::XMPlaneDot,
		::DirectX::XMPlaneDotCoord,
		::DirectX::XMPlaneDotNormal,
		::DirectX::XMPlaneEqual,
		::DirectX::XMPlaneNearEqual,
		::DirectX::XMPlaneIntersectLine,
		::DirectX::XMPlaneTransform,
		::DirectX::XMQuaternionRotationMatrix,
		::DirectX::XMQuaternionInverse,
		::DirectX::XMQuaternionIdentity,
		::DirectX::XMQuaternionSlerpV,
		::DirectX::XMQuaternionSlerp,
		::DirectX::XMQuaternionRotationAxis,
		::DirectX::XMQuaternionNormalize,
		::DirectX::XMQuaternionMultiply
		;

	namespace PackedVector
	{
		using
			::DirectX::PackedVector::XMCOLOR,
			::DirectX::PackedVector::XMHALF4,
			::DirectX::PackedVector::HALF,
			::DirectX::PackedVector::XMConvertFloatToHalf,
			::DirectX::PackedVector::XMStoreColor
			;
	}

	namespace Colors
	{
		using
			::DirectX::Colors::White,
			::DirectX::Colors::Black,
			::DirectX::Colors::Red,
			::DirectX::Colors::Green,
			::DirectX::Colors::Blue,
			::DirectX::Colors::Yellow,
			::DirectX::Colors::Cyan,
			::DirectX::Colors::Magenta,
			::DirectX::Colors::LightCoral,
			::DirectX::Colors::LightGoldenrodYellow,
			::DirectX::Colors::LightGreen,
			::DirectX::Colors::Silver,
			::DirectX::Colors::LightSteelBlue
			;
	}
}
