//! @file CYBBlock.cpp Implements functions from CYB::Engine::Block
#include "CYB.hpp"

static_assert(sizeof(CYB::Engine::Block) ==
#ifdef DEBUG
	32
#else
	16
#endif
	, "Block size has changed, check algorithms");

CYB::Engine::Block::Block(const int ASpaceAvailable, const Block& ALeftBlock, const bool AFree) :
	FMagicHeader(MAGIC_HEADER),
	FSizeAndFreeBit(InitializeData(ASpaceAvailable, AFree)),
	FOffsetToPreviousBlock(CalculateOffset(ALeftBlock)),
	FMagicFooter(MAGIC_FOOTER)
{
	API::Assert(ASpaceAvailable > sizeof(Block) || ASpaceAvailable == 0);
}

unsigned int CYB::Engine::Block::InitializeData(const int ASize, const bool AFree) {
	API::Assert(ASize >= 0);
	if(AFree)
		return ASize | 1U << 31;
	else
		return ASize & ~(1U << 31);
}

unsigned int CYB::Engine::Block::CalculateOffset(const Block& ABlock) {
	if (!ABlock.IsLargeBlock()) {
		const auto Result(static_cast<unsigned long long>(&ABlock > this ? &ABlock - this : this - &ABlock));
		API::Assert(Result <= std::numeric_limits<unsigned int>().max());
		return static_cast<unsigned int>(Result);
	}
	else
		return 0;
}

int CYB::Engine::Block::Size(void) const {
	return static_cast<int>(FSizeAndFreeBit & ~(1U << 31));
}

bool CYB::Engine::Block::IsFree(void) const {
	return (FSizeAndFreeBit >> 31) & 1;
}

bool CYB::Engine::Block::IsLargeBlock(void) const {
	return IsFree() && Size() == 0;
}

void* CYB::Engine::Block::GetData(void) {
	return this + 1;
}

CYB::Engine::Block* CYB::Engine::Block::LeftBlock(void) {
	if (FOffsetToPreviousBlock == 0)
		return nullptr;
	return reinterpret_cast<Block*>(reinterpret_cast<byte*>(this) - FOffsetToPreviousBlock);
}

CYB::Engine::Block* CYB::Engine::Block::RightBlock(void) {
	const auto CurrentSize(Size());
	API::Assert(CurrentSize > 0);
	return reinterpret_cast<Block*>(static_cast<byte*>(GetData()) + CurrentSize);
}
void CYB::Engine::Block::SetSize(const int ANewSize) {
	FSizeAndFreeBit = (ANewSize | 1U << 31);
}
void CYB::Engine::Block::SetFree(const bool ANewFree) {
	if(ANewFree)
		FSizeAndFreeBit |= 1U << 31;
	else
		FSizeAndFreeBit &= ~(1U << 31);
}