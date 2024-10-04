#include "Assertion.h"
#include "EntityManager.h"

#include "Board.h"
#include "Tetromino.h"
#include "TetrominoController.h"

TetrominoController::TetrominoController()
{
	board_ = EntityManager::Get().GetByName<Board>("Board");

	startPos_ = board_->GetStartPos();
	waitPos_ = Vec2f(150.0f, 100.0f);
	blockSize_ = 27.0f;
	blockStride_ = 30.0f;

	currentTetromino_ = Tetromino::CreateRandom(startPos_, blockSize_, blockStride_);
	currentTetromino_->status_ = Tetromino::Status::ACTIVE; /** 여기만 예외적으로 접근! */

	nextTetromino_ = Tetromino::CreateRandom(waitPos_, blockSize_, blockStride_);

	bIsInitialized_ = true;
}

TetrominoController::~TetrominoController()
{
	if (bIsInitialized_)
	{
		Release();
	}
}

void TetrominoController::Tick(float deltaSeconds)
{
	currentTetromino_->Tick(deltaSeconds);
	nextTetromino_->Tick(deltaSeconds);

	if (currentTetromino_->GetStatus() == Tetromino::Status::DONE)
	{
		const std::array<Block, Tetromino::NUM_BLOCKS>& blocks = currentTetromino_->GetBlocks();
		board_->DeployBlocks(blocks.data(), blocks.size());

		EntityManager::Get().Destroy(currentTetromino_);

		currentTetromino_ = nextTetromino_;
		currentTetromino_->startPos_ = startPos_;

		nextTetromino_ = Tetromino::CreateRandom(waitPos_, blockSize_, blockStride_);
	}
}

void TetrominoController::Render()
{
	currentTetromino_->Render();
	nextTetromino_->Render();
}

void TetrominoController::Release()
{
	CHECK(bIsInitialized_);

	if (nextTetromino_)
	{
		EntityManager::Get().Destroy(nextTetromino_);
		nextTetromino_ = nullptr;
	}

	if (currentTetromino_)
	{
		EntityManager::Get().Destroy(currentTetromino_);
		currentTetromino_ = nullptr;
	}

	board_ = nullptr;

	bIsInitialized_ = false;
}