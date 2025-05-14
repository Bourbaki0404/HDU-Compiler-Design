#pragma once

#include "common/common.hpp"
#include "IR/basicBlock.hpp"
#include "IR/instruction.hpp"
#include "IR/Value.hpp"
#include "IR/Function.hpp"
#include "common/iterator_range.hpp"

namespace IR {

// Forward declarations
class BasicBlock;
class Instruction;
class Use;

/// Iterator for users
template <typename _ValueTy>
class user_iterator {
private:
    using dlist_iterator = typename dlist<Use>::iterator;
    dlist_iterator it;
    
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = _ValueTy;
    using pointer = _ValueTy*;
    using reference = _ValueTy*;
    
    user_iterator() = default;
    user_iterator(dlist_iterator _it) : it(_it) {}
    
    bool atEnd() const { return it == dlist_iterator(); }
    
    reference operator*() const { return static_cast<reference>((*it)->inst); }
    pointer operator->() const { return static_cast<pointer>((*it)->inst); }
    
    user_iterator& operator++() { ++it; return *this; }
    user_iterator operator++(int) { user_iterator tmp = *this; ++(*this); return tmp; }
    
    bool operator==(const user_iterator& other) const { return it == other.it; }
    bool operator!=(const user_iterator& other) const { return !(*this == other); }
    
    unsigned getOperandNo() const { 
        // In a real implementation, you'd look up the operand number
        return 0; 
    }
    
    Use& getUse() const { return **it; }
};

/// BasicBlock pred_iterator implementation
template <class Ptr>
class PredIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = Ptr;
    using difference_type = std::ptrdiff_t;
    using pointer = Ptr *;
    using reference = Ptr *;
    using USE_iterator = user_iterator<Instruction>;

private:
    using Self = PredIterator<Ptr>;
    USE_iterator It;

    inline void advancePastNonTerminators() {
        // Loop to ignore non-terminator uses
        while (!It.atEnd()) {
            if (auto *Inst = dyn_cast<Instruction>(*It))
                if (Inst->isTerminator())
                    break;
            ++It;
        }
    }

public:
    PredIterator() = default;
    explicit inline PredIterator(Ptr *bb) 
        : It(bb->useList.begin()) {
        advancePastNonTerminators();
    }
    
    inline PredIterator(Ptr *bb, bool) 
        : It(bb->useList.end()) {}

    inline bool operator==(const Self& x) const { return It == x.It; }
    inline bool operator!=(const Self& x) const { return !operator==(x); }

    inline reference operator*() const {
        __assert__(!It.atEnd(), "pred_iterator out of range!");
        return dyn_cast<Instruction>(*It)->getParent();
    }
    
    inline pointer *operator->() const { return &operator*(); }

    inline Self& operator++() {
        __assert__(!It.atEnd(), "pred_iterator out of range!");
        ++It; 
        advancePastNonTerminators();
        return *this;
    }

    inline Self operator++(int) {
        Self tmp = *this; 
        ++*this; 
        return tmp;
    }

    unsigned getOperandNo() const {
        return It.getOperandNo();
    }

    Use &getUse() const {
        return It.getUse();
    }
};

using pred_iterator = PredIterator<BasicBlock>;
using const_pred_iterator = PredIterator<const BasicBlock>;
using pred_range = iterator_range<pred_iterator>;
using const_pred_range = iterator_range<const_pred_iterator>;

inline pred_iterator pred_begin(BasicBlock *BB) { return pred_iterator(BB); }
inline const_pred_iterator pred_begin(const BasicBlock *BB) {
    return const_pred_iterator(BB);
}
inline pred_iterator pred_end(BasicBlock *BB) { return pred_iterator(BB, true); }
inline const_pred_iterator pred_end(const BasicBlock *BB) {
    return const_pred_iterator(BB, true);
}
inline bool pred_empty(const BasicBlock *BB) {
    return pred_begin(BB) == pred_end(BB);
}
inline unsigned pred_size(const BasicBlock *BB) {
    return std::distance(pred_begin(BB), pred_end(BB));
}
inline pred_range predecessors(BasicBlock *BB) {
    return pred_range(pred_begin(BB), pred_end(BB));
}
inline const_pred_range predecessors(const BasicBlock *BB) {
    return const_pred_range(pred_begin(BB), pred_end(BB));
}

//===----------------------------------------------------------------------===//
// Instruction and BasicBlock succ_iterator helpers
//===----------------------------------------------------------------------===//

template <class InstructionT, class BlockT>
class SuccIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = BlockT;
    using difference_type = std::ptrdiff_t;
    using pointer = BlockT *;
    using reference = BlockT *;

private:
    InstructionT *Inst;
    int Idx;
    using Self = SuccIterator<InstructionT, BlockT>;

    inline bool index_is_valid(int Idx) {
        // Note that we specially support the index of zero being valid even in the
        // face of a null instruction.
        return Idx >= 0 && (Idx == 0 || Idx <= (int)Inst->getNumSuccessors());
    }

public:
    // begin iterator
    explicit inline SuccIterator(InstructionT *Inst) : Inst(Inst), Idx(0) {}
    // end iterator
    inline SuccIterator(InstructionT *Inst, bool) : Inst(Inst) {
        if (Inst)
            Idx = Inst->getNumSuccessors();
        else
            // Inst == NULL happens if a basic block is not fully constructed
            Idx = 0;
    }

    /// This is used to interface between code that wants to
    /// operate on terminator instructions directly.
    int getSuccessorIndex() const { return Idx; }

    inline bool operator==(const Self &x) const { return Idx == x.Idx; }
    inline bool operator!=(const Self &x) const { return !operator==(x); }

    inline BlockT *operator*() const { return Inst->getSuccessor(Idx); }

    // We use the basic block pointer directly for operator->.
    inline BlockT *operator->() const { return operator*(); }

    inline Self& operator++() {   // Preincrement
        ++Idx;
        return *this;
    }

    inline Self operator++(int) { // Postincrement
        Self tmp = *this; ++*this; return tmp;
    }

    /// Get the source BlockT of this iterator.
    inline BlockT *getSource() {
        __assert__(Inst, "Source not available, if basic block was malformed");
        return Inst->getParent();
    }
};

using succ_iterator = SuccIterator<Instruction, BasicBlock>;
using const_succ_iterator = SuccIterator<const Instruction, const BasicBlock>;
using succ_range = iterator_range<succ_iterator>;
using const_succ_range = iterator_range<const_succ_iterator>;

inline succ_iterator succ_begin(Instruction *I) { return succ_iterator(I); }
inline const_succ_iterator succ_begin(const Instruction *I) {
    return const_succ_iterator(I);
}
inline succ_iterator succ_end(Instruction *I) { return succ_iterator(I, true); }
inline const_succ_iterator succ_end(const Instruction *I) {
    return const_succ_iterator(I, true);
}
inline bool succ_empty(const Instruction *I) {
    return succ_begin(I) == succ_end(I);
}
inline unsigned succ_size(const Instruction *I) {
    return std::distance(succ_begin(I), succ_end(I));
}
inline succ_range successors(Instruction *I) {
    return succ_range(succ_begin(I), succ_end(I));
}
inline const_succ_range successors(const Instruction *I) {
    return const_succ_range(succ_begin(I), succ_end(I));
}

inline succ_iterator succ_begin(BasicBlock *BB) {
    return succ_iterator(BB->getTerminator());
}
inline const_succ_iterator succ_begin(const BasicBlock *BB) {
    return const_succ_iterator(BB->getTerminator());
}
inline succ_iterator succ_end(BasicBlock *BB) {
    return succ_iterator(BB->getTerminator(), true);
}
inline const_succ_iterator succ_end(const BasicBlock *BB) {
    return const_succ_iterator(BB->getTerminator(), true);
}
inline bool succ_empty(const BasicBlock *BB) {
    return succ_begin(BB) == succ_end(BB);
}
inline unsigned succ_size(const BasicBlock *BB) {
    return std::distance(succ_begin(BB), succ_end(BB));
}
inline succ_range successors(BasicBlock *BB) {
    return succ_range(succ_begin(BB), succ_end(BB));
}
inline const_succ_range successors(const BasicBlock *BB) {
    return const_succ_range(succ_begin(BB), succ_end(BB));
}

} // end namespace IR