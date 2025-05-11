### DAGCombiner

```cpp
//===----------------------------------------------------------------------===//
//  Main DAG Combiner implementation
//===----------------------------------------------------------------------===//

void DAGCombiner::Run(CombineLevel AtLevel) {
  // set the instance variables, so that the various visit routines may use it.
  Level = AtLevel;
  LegalDAG = Level >= AfterLegalizeDAG;
  LegalOperations = Level >= AfterLegalizeVectorOps;
  LegalTypes = Level >= AfterLegalizeTypes;

  WorklistInserter AddNodes(*this);

  // Add all the dag nodes to the worklist.
  for (SDNode &Node : DAG.allnodes())
    AddToWorklist(&Node);

  // Create a dummy node (which is not added to allnodes), that adds a reference
  // to the root node, preventing it from being deleted, and tracking any
  // changes of the root.
  HandleSDNode Dummy(DAG.getRoot());

  // While we have a valid worklist entry node, try to combine it.
  while (SDNode *N = getNextWorklistEntry()) {
    // If N has no uses, it is dead.  Make sure to revisit all N's operands once
    // N is deleted from the DAG, since they too may now be dead or may have a
    // reduced number of uses, allowing other xforms.
    if (recursivelyDeleteUnusedNodes(N))
      continue;

    WorklistRemover DeadNodes(*this);

    // If this combine is running after legalizing the DAG, re-legalize any
    // nodes pulled off the worklist.
    if (LegalDAG) {
      SmallSetVector<SDNode *, 16> UpdatedNodes;
      bool NIsValid = DAG.LegalizeOp(N, UpdatedNodes);

      for (SDNode *LN : UpdatedNodes)
        AddToWorklistWithUsers(LN);

      if (!NIsValid)
        continue;
    }

    LLVM_DEBUG(dbgs() << "\nCombining: "; N->dump(&DAG));

    // Add any operands of the new node which have not yet been combined to the
    // worklist as well. Because the worklist uniques things already, this
    // won't repeatedly process the same operand.
    CombinedNodes.insert(N);
    for (const SDValue &ChildN : N->op_values())
      if (!CombinedNodes.count(ChildN.getNode()))
        AddToWorklist(ChildN.getNode());

    SDValue RV = combine(N);

    if (!RV.getNode())
      continue;

    ++NodesCombined;

    // If we get back the same node we passed in, rather than a new node or
    // zero, we know that the node must have defined multiple values and
    // CombineTo was used.  Since CombineTo takes care of the worklist
    // mechanics for us, we have no work to do in this case.
    if (RV.getNode() == N)
      continue;

    assert(N->getOpcode() != ISD::DELETED_NODE &&
           RV.getOpcode() != ISD::DELETED_NODE &&
           "Node was deleted but visit returned new node!");

    LLVM_DEBUG(dbgs() << " ... into: "; RV.getNode()->dump(&DAG));

    if (N->getNumValues() == RV.getNode()->getNumValues())
      DAG.ReplaceAllUsesWith(N, RV.getNode());
    else {
      assert(N->getValueType(0) == RV.getValueType() &&
             N->getNumValues() == 1 && "Type mismatch");
      DAG.ReplaceAllUsesWith(N, &RV);
    }

    // Push the new node and any users onto the worklist.  Omit this if the
    // new node is the EntryToken (e.g. if a store managed to get optimized
    // out), because re-visiting the EntryToken and its users will not uncover
    // any additional opportunities, but there may be a large number of such
    // users, potentially causing compile time explosion.
    if (RV.getOpcode() != ISD::EntryToken) {
      AddToWorklist(RV.getNode());
      AddUsersToWorklist(RV.getNode());
    }

    // Finally, if the node is now dead, remove it from the graph.  The node
    // may not be dead if the replacement process recursively simplified to
    // something else needing this node. This will also take care of adding any
    // operands which have lost a user to the worklist.
    recursivelyDeleteUnusedNodes(N);
  }

  // If the root changed (e.g. it was a dead load, update the root).
  DAG.setRoot(Dummy.getValue());
  DAG.RemoveDeadNodes();
}
```

```cpp
SDValue DAGCombiner::visit(SDNode *N) {
  switch (N->getOpcode()) {
  default: break;
  case ISD::TokenFactor:        return visitTokenFactor(N);
  case ISD::MERGE_VALUES:       return visitMERGE_VALUES(N);
  case ISD::ADD:                return visitADD(N);
  case ISD::SUB:                return visitSUB(N);
  case ISD::SADDSAT:
  case ISD::UADDSAT:            return visitADDSAT(N);
  case ISD::SSUBSAT:
  case ISD::USUBSAT:            return visitSUBSAT(N);
  case ISD::ADDC:               return visitADDC(N);
  case ISD::SADDO:
  case ISD::UADDO:              return visitADDO(N);
  case ISD::SUBC:               return visitSUBC(N);
  case ISD::SSUBO:
  case ISD::USUBO:              return visitSUBO(N);
  case ISD::ADDE:               return visitADDE(N);
  case ISD::ADDCARRY:           return visitADDCARRY(N);
  case ISD::SADDO_CARRY:        return visitSADDO_CARRY(N);
  case ISD::SUBE:               return visitSUBE(N);
  case ISD::SUBCARRY:           return visitSUBCARRY(N);
  case ISD::SSUBO_CARRY:        return visitSSUBO_CARRY(N);
  case ISD::SMULFIX:
  case ISD::SMULFIXSAT:
  case ISD::UMULFIX:
  case ISD::UMULFIXSAT:         return visitMULFIX(N);
  case ISD::MUL:                return visitMUL(N);
  case ISD::SDIV:               return visitSDIV(N);
  case ISD::UDIV:               return visitUDIV(N);
  case ISD::SREM:
  case ISD::UREM:               return visitREM(N);
  case ISD::MULHU:              return visitMULHU(N);
  case ISD::MULHS:              return visitMULHS(N);
  case ISD::SMUL_LOHI:          return visitSMUL_LOHI(N);
  case ISD::UMUL_LOHI:          return visitUMUL_LOHI(N);
  case ISD::SMULO:
  case ISD::UMULO:              return visitMULO(N);
  case ISD::SMIN:
  case ISD::SMAX:
  case ISD::UMIN:
  case ISD::UMAX:               return visitIMINMAX(N);
  case ISD::AND:                return visitAND(N);
  case ISD::OR:                 return visitOR(N);
  case ISD::XOR:                return visitXOR(N);
  case ISD::SHL:                return visitSHL(N);
  case ISD::SRA:                return visitSRA(N);
  case ISD::SRL:                return visitSRL(N);
  case ISD::ROTR:
  case ISD::ROTL:               return visitRotate(N);
  case ISD::FSHL:
  case ISD::FSHR:               return visitFunnelShift(N);
  case ISD::SSHLSAT:
  case ISD::USHLSAT:            return visitSHLSAT(N);
  case ISD::ABS:                return visitABS(N);
  case ISD::BSWAP:              return visitBSWAP(N);
  case ISD::BITREVERSE:         return visitBITREVERSE(N);
  case ISD::CTLZ:               return visitCTLZ(N);
  case ISD::CTLZ_ZERO_UNDEF:    return visitCTLZ_ZERO_UNDEF(N);
  case ISD::CTTZ:               return visitCTTZ(N);
  case ISD::CTTZ_ZERO_UNDEF:    return visitCTTZ_ZERO_UNDEF(N);
  case ISD::CTPOP:              return visitCTPOP(N);
  case ISD::SELECT:             return visitSELECT(N);
  case ISD::VSELECT:            return visitVSELECT(N);
  case ISD::SELECT_CC:          return visitSELECT_CC(N);
  case ISD::SETCC:              return visitSETCC(N);
  case ISD::SETCCCARRY:         return visitSETCCCARRY(N);
  case ISD::SIGN_EXTEND:        return visitSIGN_EXTEND(N);
  case ISD::ZERO_EXTEND:        return visitZERO_EXTEND(N);
  case ISD::ANY_EXTEND:         return visitANY_EXTEND(N);
  case ISD::AssertSext:
  case ISD::AssertZext:         return visitAssertExt(N);
  case ISD::AssertAlign:        return visitAssertAlign(N);
  case ISD::SIGN_EXTEND_INREG:  return visitSIGN_EXTEND_INREG(N);
  case ISD::SIGN_EXTEND_VECTOR_INREG:
  case ISD::ZERO_EXTEND_VECTOR_INREG: return visitEXTEND_VECTOR_INREG(N);
  case ISD::TRUNCATE:           return visitTRUNCATE(N);
  case ISD::BITCAST:            return visitBITCAST(N);
  case ISD::BUILD_PAIR:         return visitBUILD_PAIR(N);
  case ISD::FADD:               return visitFADD(N);
  case ISD::STRICT_FADD:        return visitSTRICT_FADD(N);
  case ISD::FSUB:               return visitFSUB(N);
  case ISD::FMUL:               return visitFMUL(N);
  case ISD::FMA:                return visitFMA(N);
  case ISD::FDIV:               return visitFDIV(N);
  case ISD::FREM:               return visitFREM(N);
  case ISD::FSQRT:              return visitFSQRT(N);
  case ISD::FCOPYSIGN:          return visitFCOPYSIGN(N);
  case ISD::FPOW:               return visitFPOW(N);
  case ISD::SINT_TO_FP:         return visitSINT_TO_FP(N);
  case ISD::UINT_TO_FP:         return visitUINT_TO_FP(N);
  case ISD::FP_TO_SINT:         return visitFP_TO_SINT(N);
  case ISD::FP_TO_UINT:         return visitFP_TO_UINT(N);
  case ISD::FP_ROUND:           return visitFP_ROUND(N);
  case ISD::FP_EXTEND:          return visitFP_EXTEND(N);
  case ISD::FNEG:               return visitFNEG(N);
  case ISD::FABS:               return visitFABS(N);
  case ISD::FFLOOR:             return visitFFLOOR(N);
  case ISD::FMINNUM:
  case ISD::FMAXNUM:
  case ISD::FMINIMUM:
  case ISD::FMAXIMUM:           return visitFMinMax(N);
  case ISD::FCEIL:              return visitFCEIL(N);
  case ISD::FTRUNC:             return visitFTRUNC(N);
  case ISD::BRCOND:             return visitBRCOND(N);
  case ISD::BR_CC:              return visitBR_CC(N);
  case ISD::LOAD:               return visitLOAD(N);
  case ISD::STORE:              return visitSTORE(N);
  case ISD::INSERT_VECTOR_ELT:  return visitINSERT_VECTOR_ELT(N);
  case ISD::EXTRACT_VECTOR_ELT: return visitEXTRACT_VECTOR_ELT(N);
  case ISD::BUILD_VECTOR:       return visitBUILD_VECTOR(N);
  case ISD::CONCAT_VECTORS:     return visitCONCAT_VECTORS(N);
  case ISD::EXTRACT_SUBVECTOR:  return visitEXTRACT_SUBVECTOR(N);
  case ISD::VECTOR_SHUFFLE:     return visitVECTOR_SHUFFLE(N);
  case ISD::SCALAR_TO_VECTOR:   return visitSCALAR_TO_VECTOR(N);
  case ISD::INSERT_SUBVECTOR:   return visitINSERT_SUBVECTOR(N);
  case ISD::MGATHER:            return visitMGATHER(N);
  case ISD::MLOAD:              return visitMLOAD(N);
  case ISD::MSCATTER:           return visitMSCATTER(N);
  case ISD::MSTORE:             return visitMSTORE(N);
  case ISD::LIFETIME_END:       return visitLIFETIME_END(N);
  case ISD::FP_TO_FP16:         return visitFP_TO_FP16(N);
  case ISD::FP16_TO_FP:         return visitFP16_TO_FP(N);
  case ISD::FREEZE:             return visitFREEZE(N);
  case ISD::VECREDUCE_FADD:
  case ISD::VECREDUCE_FMUL:
  case ISD::VECREDUCE_ADD:
  case ISD::VECREDUCE_MUL:
  case ISD::VECREDUCE_AND:
  case ISD::VECREDUCE_OR:
  case ISD::VECREDUCE_XOR:
  case ISD::VECREDUCE_SMAX:
  case ISD::VECREDUCE_SMIN:
  case ISD::VECREDUCE_UMAX:
  case ISD::VECREDUCE_UMIN:
  case ISD::VECREDUCE_FMAX:
  case ISD::VECREDUCE_FMIN:     return visitVECREDUCE(N);
#define BEGIN_REGISTER_VP_SDNODE(SDOPC, ...) case ISD::SDOPC:
#include "llvm/IR/VPIntrinsics.def"
    return visitVPOp(N);
  }
  return SDValue();
}
```

```cpp
SDValue DAGCombiner::combine(SDNode *N) {
  SDValue RV;
  if (!DisableGenericCombines)
    RV = visit(N);

  // If nothing happened, try a target-specific DAG combine.
  if (!RV.getNode()) {
    assert(N->getOpcode() != ISD::DELETED_NODE &&
           "Node was deleted but visit returned NULL!");

    if (N->getOpcode() >= ISD::BUILTIN_OP_END ||
        TLI.hasTargetDAGCombine((ISD::NodeType)N->getOpcode())) {

      // Expose the DAG combiner to the target combiner impls.
      TargetLowering::DAGCombinerInfo
        DagCombineInfo(DAG, Level, false, this);

      RV = TLI.PerformDAGCombine(N, DagCombineInfo);
    }
  }

  // If nothing happened still, try promoting the operation.
  if (!RV.getNode()) {
    switch (N->getOpcode()) {
    default: break;
    case ISD::ADD:
    case ISD::SUB:
    case ISD::MUL:
    case ISD::AND:
    case ISD::OR:
    case ISD::XOR:
      RV = PromoteIntBinOp(SDValue(N, 0));
      break;
    case ISD::SHL:
    case ISD::SRA:
    case ISD::SRL:
      RV = PromoteIntShiftOp(SDValue(N, 0));
      break;
    case ISD::SIGN_EXTEND:
    case ISD::ZERO_EXTEND:
    case ISD::ANY_EXTEND:
      RV = PromoteExtend(SDValue(N, 0));
      break;
    case ISD::LOAD:
      if (PromoteLoad(SDValue(N, 0)))
        RV = SDValue(N, 0);
      break;
    }
  }

  // If N is a commutative binary node, try to eliminate it if the commuted
  // version is already present in the DAG.
  if (!RV.getNode() && TLI.isCommutativeBinOp(N->getOpcode()) &&
      N->getNumValues() == 1) {
    SDValue N0 = N->getOperand(0);
    SDValue N1 = N->getOperand(1);

    // Constant operands are canonicalized to RHS.
    if (N0 != N1 && (isa<ConstantSDNode>(N0) || !isa<ConstantSDNode>(N1))) {
      SDValue Ops[] = {N1, N0};
      SDNode *CSENode = DAG.getNodeIfExists(N->getOpcode(), N->getVTList(), Ops,
                                            N->getFlags());
      if (CSENode)
        return SDValue(CSENode, 0);
    }
  }

  return RV;
}
```
```cpp
SDValue DAGCombiner::visitMUL(SDNode *N) {
  SDValue N0 = N->getOperand(0);
  SDValue N1 = N->getOperand(1);
  EVT VT = N0.getValueType();

  // fold (mul x, undef) -> 0
  if (N0.isUndef() || N1.isUndef())
    return DAG.getConstant(0, SDLoc(N), VT);

  // fold (mul c1, c2) -> c1*c2
  if (SDValue C = DAG.FoldConstantArithmetic(ISD::MUL, SDLoc(N), VT, {N0, N1}))
    return C;

  // canonicalize constant to RHS (vector doesn't have to splat)
  if (DAG.isConstantIntBuildVectorOrConstantInt(N0) &&
      !DAG.isConstantIntBuildVectorOrConstantInt(N1))
    return DAG.getNode(ISD::MUL, SDLoc(N), VT, N1, N0);

  bool N1IsConst = false;
  bool N1IsOpaqueConst = false;
  APInt ConstValue1;

  // fold vector ops
  if (VT.isVector()) {
    if (SDValue FoldedVOp = SimplifyVBinOp(N, SDLoc(N)))
      return FoldedVOp;

    N1IsConst = ISD::isConstantSplatVector(N1.getNode(), ConstValue1);
    assert((!N1IsConst ||
            ConstValue1.getBitWidth() == VT.getScalarSizeInBits()) &&
           "Splat APInt should be element width");
  } else {
    N1IsConst = isa<ConstantSDNode>(N1);
    if (N1IsConst) {
      ConstValue1 = cast<ConstantSDNode>(N1)->getAPIntValue();
      N1IsOpaqueConst = cast<ConstantSDNode>(N1)->isOpaque();
    }
  }

  // fold (mul x, 0) -> 0
  if (N1IsConst && ConstValue1.isZero())
    return N1;

  // fold (mul x, 1) -> x
  if (N1IsConst && ConstValue1.isOne())
    return N0;

  if (SDValue NewSel = foldBinOpIntoSelect(N))
    return NewSel;

  // fold (mul x, -1) -> 0-x
  if (N1IsConst && ConstValue1.isAllOnes()) {
    SDLoc DL(N);
    return DAG.getNode(ISD::SUB, DL, VT,
                       DAG.getConstant(0, DL, VT), N0);
  }

  // fold (mul x, (1 << c)) -> x << c
  if (isConstantOrConstantVector(N1, /*NoOpaques*/ true) &&
      DAG.isKnownToBeAPowerOfTwo(N1) &&
      (!VT.isVector() || Level <= AfterLegalizeVectorOps)) {
    SDLoc DL(N);
    SDValue LogBase2 = BuildLogBase2(N1, DL);
    EVT ShiftVT = getShiftAmountTy(N0.getValueType());
    SDValue Trunc = DAG.getZExtOrTrunc(LogBase2, DL, ShiftVT);
    return DAG.getNode(ISD::SHL, DL, VT, N0, Trunc);
  }

  // fold (mul x, -(1 << c)) -> -(x << c) or (-x) << c
  if (N1IsConst && !N1IsOpaqueConst && ConstValue1.isNegatedPowerOf2()) {
    unsigned Log2Val = (-ConstValue1).logBase2();
    SDLoc DL(N);
    // FIXME: If the input is something that is easily negated (e.g. a
    // single-use add), we should put the negate there.
    return DAG.getNode(ISD::SUB, DL, VT,
                       DAG.getConstant(0, DL, VT),
                       DAG.getNode(ISD::SHL, DL, VT, N0,
                            DAG.getConstant(Log2Val, DL,
                                      getShiftAmountTy(N0.getValueType()))));
  }

  // Try to transform:
  // (1) multiply-by-(power-of-2 +/- 1) into shift and add/sub.
  // mul x, (2^N + 1) --> add (shl x, N), x
  // mul x, (2^N - 1) --> sub (shl x, N), x
  // Examples: x * 33 --> (x << 5) + x
  //           x * 15 --> (x << 4) - x
  //           x * -33 --> -((x << 5) + x)
  //           x * -15 --> -((x << 4) - x) ; this reduces --> x - (x << 4)
  // (2) multiply-by-(power-of-2 +/- power-of-2) into shifts and add/sub.
  // mul x, (2^N + 2^M) --> (add (shl x, N), (shl x, M))
  // mul x, (2^N - 2^M) --> (sub (shl x, N), (shl x, M))
  // Examples: x * 0x8800 --> (x << 15) + (x << 11)
  //           x * 0xf800 --> (x << 16) - (x << 11)
  //           x * -0x8800 --> -((x << 15) + (x << 11))
  //           x * -0xf800 --> -((x << 16) - (x << 11)) ; (x << 11) - (x << 16)
  if (N1IsConst && TLI.decomposeMulByConstant(*DAG.getContext(), VT, N1)) {
    // TODO: We could handle more general decomposition of any constant by
    //       having the target set a limit on number of ops and making a
    //       callback to determine that sequence (similar to sqrt expansion).
    unsigned MathOp = ISD::DELETED_NODE;
    APInt MulC = ConstValue1.abs();
    // The constant `2` should be treated as (2^0 + 1).
    unsigned TZeros = MulC == 2 ? 0 : MulC.countTrailingZeros();
    MulC.lshrInPlace(TZeros);
    if ((MulC - 1).isPowerOf2())
      MathOp = ISD::ADD;
    else if ((MulC + 1).isPowerOf2())
      MathOp = ISD::SUB;

    if (MathOp != ISD::DELETED_NODE) {
      unsigned ShAmt =
          MathOp == ISD::ADD ? (MulC - 1).logBase2() : (MulC + 1).logBase2();
      ShAmt += TZeros;
      assert(ShAmt < VT.getScalarSizeInBits() &&
             "multiply-by-constant generated out of bounds shift");
      SDLoc DL(N);
      SDValue Shl =
          DAG.getNode(ISD::SHL, DL, VT, N0, DAG.getConstant(ShAmt, DL, VT));
      SDValue R =
          TZeros ? DAG.getNode(MathOp, DL, VT, Shl,
                               DAG.getNode(ISD::SHL, DL, VT, N0,
                                           DAG.getConstant(TZeros, DL, VT)))
                 : DAG.getNode(MathOp, DL, VT, Shl, N0);
      if (ConstValue1.isNegative())
        R = DAG.getNode(ISD::SUB, DL, VT, DAG.getConstant(0, DL, VT), R);
      return R;
    }
  }

  // (mul (shl X, c1), c2) -> (mul X, c2 << c1)
  if (N0.getOpcode() == ISD::SHL &&
      isConstantOrConstantVector(N1, /* NoOpaques */ true) &&
      isConstantOrConstantVector(N0.getOperand(1), /* NoOpaques */ true)) {
    SDValue C3 = DAG.getNode(ISD::SHL, SDLoc(N), VT, N1, N0.getOperand(1));
    if (isConstantOrConstantVector(C3))
      return DAG.getNode(ISD::MUL, SDLoc(N), VT, N0.getOperand(0), C3);
  }

  // Change (mul (shl X, C), Y) -> (shl (mul X, Y), C) when the shift has one
  // use.
  {
    SDValue Sh, Y;

    // Check for both (mul (shl X, C), Y)  and  (mul Y, (shl X, C)).
    if (N0.getOpcode() == ISD::SHL &&
        isConstantOrConstantVector(N0.getOperand(1)) &&
        N0.getNode()->hasOneUse()) {
      Sh = N0; Y = N1;
    } else if (N1.getOpcode() == ISD::SHL &&
               isConstantOrConstantVector(N1.getOperand(1)) &&
               N1.getNode()->hasOneUse()) {
      Sh = N1; Y = N0;
    }

    if (Sh.getNode()) {
      SDValue Mul = DAG.getNode(ISD::MUL, SDLoc(N), VT, Sh.getOperand(0), Y);
      return DAG.getNode(ISD::SHL, SDLoc(N), VT, Mul, Sh.getOperand(1));
    }
  }

  // fold (mul (add x, c1), c2) -> (add (mul x, c2), c1*c2)
  if (DAG.isConstantIntBuildVectorOrConstantInt(N1) &&
      N0.getOpcode() == ISD::ADD &&
      DAG.isConstantIntBuildVectorOrConstantInt(N0.getOperand(1)) &&
      isMulAddWithConstProfitable(N, N0, N1))
      return DAG.getNode(ISD::ADD, SDLoc(N), VT,
                         DAG.getNode(ISD::MUL, SDLoc(N0), VT,
                                     N0.getOperand(0), N1),
                         DAG.getNode(ISD::MUL, SDLoc(N1), VT,
                                     N0.getOperand(1), N1));

  // Fold (mul (vscale * C0), C1) to (vscale * (C0 * C1)).
  if (N0.getOpcode() == ISD::VSCALE)
    if (ConstantSDNode *NC1 = isConstOrConstSplat(N1)) {
      const APInt &C0 = N0.getConstantOperandAPInt(0);
      const APInt &C1 = NC1->getAPIntValue();
      return DAG.getVScale(SDLoc(N), VT, C0 * C1);
    }

  // Fold (mul step_vector(C0), C1) to (step_vector(C0 * C1)).
  APInt MulVal;
  if (N0.getOpcode() == ISD::STEP_VECTOR)
    if (ISD::isConstantSplatVector(N1.getNode(), MulVal)) {
      const APInt &C0 = N0.getConstantOperandAPInt(0);
      APInt NewStep = C0 * MulVal;
      return DAG.getStepVector(SDLoc(N), VT, NewStep);
    }

  // Fold ((mul x, 0/undef) -> 0,
  //       (mul x, 1) -> x) -> x)
  // -> and(x, mask)
  // We can replace vectors with '0' and '1' factors with a clearing mask.
  if (VT.isFixedLengthVector()) {
    unsigned NumElts = VT.getVectorNumElements();
    SmallBitVector ClearMask;
    ClearMask.reserve(NumElts);
    auto IsClearMask = [&ClearMask](ConstantSDNode *V) {
      if (!V || V->isZero()) {
        ClearMask.push_back(true);
        return true;
      }
      ClearMask.push_back(false);
      return V->isOne();
    };
    if ((!LegalOperations || TLI.isOperationLegalOrCustom(ISD::AND, VT)) &&
        ISD::matchUnaryPredicate(N1, IsClearMask, /*AllowUndefs*/ true)) {
      assert(N1.getOpcode() == ISD::BUILD_VECTOR && "Unknown constant vector");
      SDLoc DL(N);
      EVT LegalSVT = N1.getOperand(0).getValueType();
      SDValue Zero = DAG.getConstant(0, DL, LegalSVT);
      SDValue AllOnes = DAG.getAllOnesConstant(DL, LegalSVT);
      SmallVector<SDValue, 16> Mask(NumElts, AllOnes);
      for (unsigned I = 0; I != NumElts; ++I)
        if (ClearMask[I])
          Mask[I] = Zero;
      return DAG.getNode(ISD::AND, DL, VT, N0, DAG.getBuildVector(VT, DL, Mask));
    }
  }

  // reassociate mul
  if (SDValue RMUL = reassociateOps(ISD::MUL, SDLoc(N), N0, N1, N->getFlags()))
    return RMUL;

  return SDValue();
}
```
```cpp
static SDValue simplifyDivRem(SDNode *N, SelectionDAG &DAG) {
  SDValue N0 = N->getOperand(0);
  SDValue N1 = N->getOperand(1);
  EVT VT = N->getValueType(0);
  SDLoc DL(N);

  unsigned Opc = N->getOpcode();
  bool IsDiv = (ISD::SDIV == Opc) || (ISD::UDIV == Opc);
  ConstantSDNode *N1C = isConstOrConstSplat(N1);

  // X / undef -> undef
  // X % undef -> undef
  // X / 0 -> undef
  // X % 0 -> undef
  // NOTE: This includes vectors where any divisor element is zero/undef.
  if (DAG.isUndef(Opc, {N0, N1}))
    return DAG.getUNDEF(VT);

  // undef / X -> 0
  // undef % X -> 0
  if (N0.isUndef())
    return DAG.getConstant(0, DL, VT);

  // 0 / X -> 0
  // 0 % X -> 0
  ConstantSDNode *N0C = isConstOrConstSplat(N0);
  if (N0C && N0C->isZero())
    return N0;

  // X / X -> 1
  // X % X -> 0
  if (N0 == N1)
    return DAG.getConstant(IsDiv ? 1 : 0, DL, VT);

  // X / 1 -> X
  // X % 1 -> 0
  // If this is a boolean op (single-bit element type), we can't have
  // division-by-zero or remainder-by-zero, so assume the divisor is 1.
  // TODO: Similarly, if we're zero-extending a boolean divisor, then assume
  // it's a 1.
  if ((N1C && N1C->isOne()) || (VT.getScalarType() == MVT::i1))
    return IsDiv ? N0 : DAG.getConstant(0, DL, VT);

  return SDValue();
}
```

```cpp
SDValue DAGCombiner::visitADD(SDNode *N) {
  SDValue N0 = N->getOperand(0);
  SDValue N1 = N->getOperand(1);
  EVT VT = N0.getValueType();
  SDLoc DL(N);

  if (SDValue Combined = visitADDLike(N))
    return Combined;

  if (SDValue V = foldAddSubBoolOfMaskedVal(N, DAG))
    return V;

  if (SDValue V = foldAddSubOfSignBit(N, DAG))
    return V;

  // fold (a+b) -> (a|b) iff a and b share no bits.
  if ((!LegalOperations || TLI.isOperationLegal(ISD::OR, VT)) &&
      DAG.haveNoCommonBitsSet(N0, N1))
    return DAG.getNode(ISD::OR, DL, VT, N0, N1);

  // Fold (add (vscale * C0), (vscale * C1)) to (vscale * (C0 + C1)).
  if (N0.getOpcode() == ISD::VSCALE && N1.getOpcode() == ISD::VSCALE) {
    const APInt &C0 = N0->getConstantOperandAPInt(0);
    const APInt &C1 = N1->getConstantOperandAPInt(0);
    return DAG.getVScale(DL, VT, C0 + C1);
  }

  // fold a+vscale(c1)+vscale(c2) -> a+vscale(c1+c2)
  if ((N0.getOpcode() == ISD::ADD) &&
      (N0.getOperand(1).getOpcode() == ISD::VSCALE) &&
      (N1.getOpcode() == ISD::VSCALE)) {
    const APInt &VS0 = N0.getOperand(1)->getConstantOperandAPInt(0);
    const APInt &VS1 = N1->getConstantOperandAPInt(0);
    SDValue VS = DAG.getVScale(DL, VT, VS0 + VS1);
    return DAG.getNode(ISD::ADD, DL, VT, N0.getOperand(0), VS);
  }

  // Fold (add step_vector(c1), step_vector(c2)  to step_vector(c1+c2))
  if (N0.getOpcode() == ISD::STEP_VECTOR &&
      N1.getOpcode() == ISD::STEP_VECTOR) {
    const APInt &C0 = N0->getConstantOperandAPInt(0);
    const APInt &C1 = N1->getConstantOperandAPInt(0);
    APInt NewStep = C0 + C1;
    return DAG.getStepVector(DL, VT, NewStep);
  }

  // Fold a + step_vector(c1) + step_vector(c2) to a + step_vector(c1+c2)
  if ((N0.getOpcode() == ISD::ADD) &&
      (N0.getOperand(1).getOpcode() == ISD::STEP_VECTOR) &&
      (N1.getOpcode() == ISD::STEP_VECTOR)) {
    const APInt &SV0 = N0.getOperand(1)->getConstantOperandAPInt(0);
    const APInt &SV1 = N1->getConstantOperandAPInt(0);
    APInt NewStep = SV0 + SV1;
    SDValue SV = DAG.getStepVector(DL, VT, NewStep);
    return DAG.getNode(ISD::ADD, DL, VT, N0.getOperand(0), SV);
  }

  return SDValue();
}
```

```cpp
SDValue DAGCombiner::visitTRUNCATE(SDNode *N) {
  SDValue N0 = N->getOperand(0);
  EVT VT = N->getValueType(0);
  EVT SrcVT = N0.getValueType();
  bool isLE = DAG.getDataLayout().isLittleEndian();

  // noop truncate
  if (SrcVT == VT)
    return N0;

  // fold (truncate (truncate x)) -> (truncate x)
  if (N0.getOpcode() == ISD::TRUNCATE)
    return DAG.getNode(ISD::TRUNCATE, SDLoc(N), VT, N0.getOperand(0));

  // fold (truncate c1) -> c1
  if (DAG.isConstantIntBuildVectorOrConstantInt(N0)) {
    SDValue C = DAG.getNode(ISD::TRUNCATE, SDLoc(N), VT, N0);
    if (C.getNode() != N)
      return C;
  }

  // fold (truncate (ext x)) -> (ext x) or (truncate x) or x
  if (N0.getOpcode() == ISD::ZERO_EXTEND ||
      N0.getOpcode() == ISD::SIGN_EXTEND ||
      N0.getOpcode() == ISD::ANY_EXTEND) {
    // if the source is smaller than the dest, we still need an extend.
    if (N0.getOperand(0).getValueType().bitsLT(VT))
      return DAG.getNode(N0.getOpcode(), SDLoc(N), VT, N0.getOperand(0));
    // if the source is larger than the dest, than we just need the truncate.
    if (N0.getOperand(0).getValueType().bitsGT(VT))
      return DAG.getNode(ISD::TRUNCATE, SDLoc(N), VT, N0.getOperand(0));
    // if the source and dest are the same type, we can drop both the extend
    // and the truncate.
    return N0.getOperand(0);
  }

  // If this is anyext(trunc), don't fold it, allow ourselves to be folded.
  if (N->hasOneUse() && (N->use_begin()->getOpcode() == ISD::ANY_EXTEND))
    return SDValue();

  // Fold extract-and-trunc into a narrow extract. For example:
  //   i64 x = EXTRACT_VECTOR_ELT(v2i64 val, i32 1)
  //   i32 y = TRUNCATE(i64 x)
  //        -- becomes --
  //   v16i8 b = BITCAST (v2i64 val)
  //   i8 x = EXTRACT_VECTOR_ELT(v16i8 b, i32 8)
  //
  // Note: We only run this optimization after type legalization (which often
  // creates this pattern) and before operation legalization after which
  // we need to be more careful about the vector instructions that we generate.
  if (N0.getOpcode() == ISD::EXTRACT_VECTOR_ELT &&
      LegalTypes && !LegalOperations && N0->hasOneUse() && VT != MVT::i1) {
    EVT VecTy = N0.getOperand(0).getValueType();
    EVT ExTy = N0.getValueType();
    EVT TrTy = N->getValueType(0);

    auto EltCnt = VecTy.getVectorElementCount();
    unsigned SizeRatio = ExTy.getSizeInBits()/TrTy.getSizeInBits();
    auto NewEltCnt = EltCnt * SizeRatio;

    EVT NVT = EVT::getVectorVT(*DAG.getContext(), TrTy, NewEltCnt);
    assert(NVT.getSizeInBits() == VecTy.getSizeInBits() && "Invalid Size");

    SDValue EltNo = N0->getOperand(1);
    if (isa<ConstantSDNode>(EltNo) && isTypeLegal(NVT)) {
      int Elt = cast<ConstantSDNode>(EltNo)->getZExtValue();
      int Index = isLE ? (Elt*SizeRatio) : (Elt*SizeRatio + (SizeRatio-1));

      SDLoc DL(N);
      return DAG.getNode(ISD::EXTRACT_VECTOR_ELT, DL, TrTy,
                         DAG.getBitcast(NVT, N0.getOperand(0)),
                         DAG.getVectorIdxConstant(Index, DL));
    }
  }

  // trunc (select c, a, b) -> select c, (trunc a), (trunc b)
  if (N0.getOpcode() == ISD::SELECT && N0.hasOneUse()) {
    if ((!LegalOperations || TLI.isOperationLegal(ISD::SELECT, SrcVT)) &&
        TLI.isTruncateFree(SrcVT, VT)) {
      SDLoc SL(N0);
      SDValue Cond = N0.getOperand(0);
      SDValue TruncOp0 = DAG.getNode(ISD::TRUNCATE, SL, VT, N0.getOperand(1));
      SDValue TruncOp1 = DAG.getNode(ISD::TRUNCATE, SL, VT, N0.getOperand(2));
      return DAG.getNode(ISD::SELECT, SDLoc(N), VT, Cond, TruncOp0, TruncOp1);
    }
  }

  // trunc (shl x, K) -> shl (trunc x), K => K < VT.getScalarSizeInBits()
  if (N0.getOpcode() == ISD::SHL && N0.hasOneUse() &&
      (!LegalOperations || TLI.isOperationLegal(ISD::SHL, VT)) &&
      TLI.isTypeDesirableForOp(ISD::SHL, VT)) {
    SDValue Amt = N0.getOperand(1);
    KnownBits Known = DAG.computeKnownBits(Amt);
    unsigned Size = VT.getScalarSizeInBits();
    if (Known.countMaxActiveBits() <= Log2_32(Size)) {
      SDLoc SL(N);
      EVT AmtVT = TLI.getShiftAmountTy(VT, DAG.getDataLayout());

      SDValue Trunc = DAG.getNode(ISD::TRUNCATE, SL, VT, N0.getOperand(0));
      if (AmtVT != Amt.getValueType()) {
        Amt = DAG.getZExtOrTrunc(Amt, SL, AmtVT);
        AddToWorklist(Amt.getNode());
      }
      return DAG.getNode(ISD::SHL, SL, VT, Trunc, Amt);
    }
  }

  if (SDValue V = foldSubToUSubSat(VT, N0.getNode()))
    return V;

  // Attempt to pre-truncate BUILD_VECTOR sources.
  if (N0.getOpcode() == ISD::BUILD_VECTOR && !LegalOperations &&
      TLI.isTruncateFree(SrcVT.getScalarType(), VT.getScalarType()) &&
      // Avoid creating illegal types if running after type legalizer.
      (!LegalTypes || TLI.isTypeLegal(VT.getScalarType()))) {
    SDLoc DL(N);
    EVT SVT = VT.getScalarType();
    SmallVector<SDValue, 8> TruncOps;
    for (const SDValue &Op : N0->op_values()) {
      SDValue TruncOp = DAG.getNode(ISD::TRUNCATE, DL, SVT, Op);
      TruncOps.push_back(TruncOp);
    }
    return DAG.getBuildVector(VT, DL, TruncOps);
  }

  // Fold a series of buildvector, bitcast, and truncate if possible.
  // For example fold
  //   (2xi32 trunc (bitcast ((4xi32)buildvector x, x, y, y) 2xi64)) to
  //   (2xi32 (buildvector x, y)).
  if (Level == AfterLegalizeVectorOps && VT.isVector() &&
      N0.getOpcode() == ISD::BITCAST && N0.hasOneUse() &&
      N0.getOperand(0).getOpcode() == ISD::BUILD_VECTOR &&
      N0.getOperand(0).hasOneUse()) {
    SDValue BuildVect = N0.getOperand(0);
    EVT BuildVectEltTy = BuildVect.getValueType().getVectorElementType();
    EVT TruncVecEltTy = VT.getVectorElementType();

    // Check that the element types match.
    if (BuildVectEltTy == TruncVecEltTy) {
      // Now we only need to compute the offset of the truncated elements.
      unsigned BuildVecNumElts =  BuildVect.getNumOperands();
      unsigned TruncVecNumElts = VT.getVectorNumElements();
      unsigned TruncEltOffset = BuildVecNumElts / TruncVecNumElts;

      assert((BuildVecNumElts % TruncVecNumElts) == 0 &&
             "Invalid number of elements");

      SmallVector<SDValue, 8> Opnds;
      for (unsigned i = 0, e = BuildVecNumElts; i != e; i += TruncEltOffset)
        Opnds.push_back(BuildVect.getOperand(i));

      return DAG.getBuildVector(VT, SDLoc(N), Opnds);
    }
  }

  // See if we can simplify the input to this truncate through knowledge that
  // only the low bits are being used.
  // For example "trunc (or (shl x, 8), y)" // -> trunc y
  // Currently we only perform this optimization on scalars because vectors
  // may have different active low bits.
  if (!VT.isVector()) {
    APInt Mask =
        APInt::getLowBitsSet(N0.getValueSizeInBits(), VT.getSizeInBits());
    if (SDValue Shorter = DAG.GetDemandedBits(N0, Mask))
      return DAG.getNode(ISD::TRUNCATE, SDLoc(N), VT, Shorter);
  }

  // fold (truncate (load x)) -> (smaller load x)
  // fold (truncate (srl (load x), c)) -> (smaller load (x+c/evtbits))
  if (!LegalTypes || TLI.isTypeDesirableForOp(N0.getOpcode(), VT)) {
    if (SDValue Reduced = reduceLoadWidth(N))
      return Reduced;

    // Handle the case where the load remains an extending load even
    // after truncation.
    if (N0.hasOneUse() && ISD::isUNINDEXEDLoad(N0.getNode())) {
      LoadSDNode *LN0 = cast<LoadSDNode>(N0);
      if (LN0->isSimple() && LN0->getMemoryVT().bitsLT(VT)) {
        SDValue NewLoad = DAG.getExtLoad(LN0->getExtensionType(), SDLoc(LN0),
                                         VT, LN0->getChain(), LN0->getBasePtr(),
                                         LN0->getMemoryVT(),
                                         LN0->getMemOperand());
        DAG.ReplaceAllUsesOfValueWith(N0.getValue(1), NewLoad.getValue(1));
        return NewLoad;
      }
    }
  }

  // fold (trunc (concat ... x ...)) -> (concat ..., (trunc x), ...)),
  // where ... are all 'undef'.
  if (N0.getOpcode() == ISD::CONCAT_VECTORS && !LegalTypes) {
    SmallVector<EVT, 8> VTs;
    SDValue V;
    unsigned Idx = 0;
    unsigned NumDefs = 0;

    for (unsigned i = 0, e = N0.getNumOperands(); i != e; ++i) {
      SDValue X = N0.getOperand(i);
      if (!X.isUndef()) {
        V = X;
        Idx = i;
        NumDefs++;
      }
      // Stop if more than one members are non-undef.
      if (NumDefs > 1)
        break;

      VTs.push_back(EVT::getVectorVT(*DAG.getContext(),
                                     VT.getVectorElementType(),
                                     X.getValueType().getVectorElementCount()));
    }

    if (NumDefs == 0)
      return DAG.getUNDEF(VT);

    if (NumDefs == 1) {
      assert(V.getNode() && "The single defined operand is empty!");
      SmallVector<SDValue, 8> Opnds;
      for (unsigned i = 0, e = VTs.size(); i != e; ++i) {
        if (i != Idx) {
          Opnds.push_back(DAG.getUNDEF(VTs[i]));
          continue;
        }
        SDValue NV = DAG.getNode(ISD::TRUNCATE, SDLoc(V), VTs[i], V);
        AddToWorklist(NV.getNode());
        Opnds.push_back(NV);
      }
      return DAG.getNode(ISD::CONCAT_VECTORS, SDLoc(N), VT, Opnds);
    }
  }

  // Fold truncate of a bitcast of a vector to an extract of the low vector
  // element.
  //
  // e.g. trunc (i64 (bitcast v2i32:x)) -> extract_vector_elt v2i32:x, idx
  if (N0.getOpcode() == ISD::BITCAST && !VT.isVector()) {
    SDValue VecSrc = N0.getOperand(0);
    EVT VecSrcVT = VecSrc.getValueType();
    if (VecSrcVT.isVector() && VecSrcVT.getScalarType() == VT &&
        (!LegalOperations ||
         TLI.isOperationLegal(ISD::EXTRACT_VECTOR_ELT, VecSrcVT))) {
      SDLoc SL(N);

      unsigned Idx = isLE ? 0 : VecSrcVT.getVectorNumElements() - 1;
      return DAG.getNode(ISD::EXTRACT_VECTOR_ELT, SL, VT, VecSrc,
                         DAG.getVectorIdxConstant(Idx, SL));
    }
  }

  // Simplify the operands using demanded-bits information.
  if (SimplifyDemandedBits(SDValue(N, 0)))
    return SDValue(N, 0);

  // (trunc adde(X, Y, Carry)) -> (adde trunc(X), trunc(Y), Carry)
  // (trunc addcarry(X, Y, Carry)) -> (addcarry trunc(X), trunc(Y), Carry)
  // When the adde's carry is not used.
  if ((N0.getOpcode() == ISD::ADDE || N0.getOpcode() == ISD::ADDCARRY) &&
      N0.hasOneUse() && !N0.getNode()->hasAnyUseOfValue(1) &&
      // We only do for addcarry before legalize operation
      ((!LegalOperations && N0.getOpcode() == ISD::ADDCARRY) ||
       TLI.isOperationLegal(N0.getOpcode(), VT))) {
    SDLoc SL(N);
    auto X = DAG.getNode(ISD::TRUNCATE, SL, VT, N0.getOperand(0));
    auto Y = DAG.getNode(ISD::TRUNCATE, SL, VT, N0.getOperand(1));
    auto VTs = DAG.getVTList(VT, N0->getValueType(1));
    return DAG.getNode(N0.getOpcode(), SL, VTs, X, Y, N0.getOperand(2));
  }

  // fold (truncate (extract_subvector(ext x))) ->
  //      (extract_subvector x)
  // TODO: This can be generalized to cover cases where the truncate and extract
  // do not fully cancel each other out.
  if (!LegalTypes && N0.getOpcode() == ISD::EXTRACT_SUBVECTOR) {
    SDValue N00 = N0.getOperand(0);
    if (N00.getOpcode() == ISD::SIGN_EXTEND ||
        N00.getOpcode() == ISD::ZERO_EXTEND ||
        N00.getOpcode() == ISD::ANY_EXTEND) {
      if (N00.getOperand(0)->getValueType(0).getVectorElementType() ==
          VT.getVectorElementType())
        return DAG.getNode(ISD::EXTRACT_SUBVECTOR, SDLoc(N0->getOperand(0)), VT,
                           N00.getOperand(0), N0.getOperand(1));
    }
  }

  if (SDValue NewVSel = matchVSelectOpSizesWithSetCC(N))
    return NewVSel;

  // Narrow a suitable binary operation with a non-opaque constant operand by
  // moving it ahead of the truncate. This is limited to pre-legalization
  // because targets may prefer a wider type during later combines and invert
  // this transform.
  switch (N0.getOpcode()) {
  case ISD::ADD:
  case ISD::SUB:
  case ISD::MUL:
  case ISD::AND:
  case ISD::OR:
  case ISD::XOR:
    if (!LegalOperations && N0.hasOneUse() &&
        (isConstantOrConstantVector(N0.getOperand(0), true) ||
         isConstantOrConstantVector(N0.getOperand(1), true))) {
      // TODO: We already restricted this to pre-legalization, but for vectors
      // we are extra cautious to not create an unsupported operation.
      // Target-specific changes are likely needed to avoid regressions here.
      if (VT.isScalarInteger() || TLI.isOperationLegal(N0.getOpcode(), VT)) {
        SDLoc DL(N);
        SDValue NarrowL = DAG.getNode(ISD::TRUNCATE, DL, VT, N0.getOperand(0));
        SDValue NarrowR = DAG.getNode(ISD::TRUNCATE, DL, VT, N0.getOperand(1));
        return DAG.getNode(N0.getOpcode(), DL, VT, NarrowL, NarrowR);
      }
    }
    break;
  case ISD::USUBSAT:
    // Truncate the USUBSAT only if LHS is a known zero-extension, its not
    // enough to know that the upper bits are zero we must ensure that we don't
    // introduce an extra truncate.
    if (!LegalOperations && N0.hasOneUse() &&
        N0.getOperand(0).getOpcode() == ISD::ZERO_EXTEND &&
        N0.getOperand(0).getOperand(0).getScalarValueSizeInBits() <=
            VT.getScalarSizeInBits() &&
        hasOperation(N0.getOpcode(), VT)) {
      return getTruncatedUSUBSAT(VT, SrcVT, N0.getOperand(0), N0.getOperand(1),
                                 DAG, SDLoc(N));
    }
    break;
  }

  return SDValue();
}
```
This document explains the `visitTRUNCATE` function in LLVM's DAG combiner, which handles optimization of truncation operations during instruction selection.

## Key Concepts

- **TRUNCATE**: Reduces bit width (e.g., i64 → i32)
- **DAG Combiner**: Optimizes SelectionDAG with target-independent and target-specific patterns

## Optimization Patterns

### 1. No-op Truncate
```cpp
SDValue DAGCombiner::visitTokenFactor(SDNode *N) {
  // If N has two operands, where one has an input chain equal to the other,
  // the 'other' chain is redundant.
  if (N->getNumOperands() == 2) {
    if (getInputChainForNode(N->getOperand(0).getNode()) == N->getOperand(1))
      return N->getOperand(0);
    if (getInputChainForNode(N->getOperand(1).getNode()) == N->getOperand(0))
      return N->getOperand(1);
  }

  // Don't simplify token factors if optnone.
  if (OptLevel == CodeGenOpt::None)
    return SDValue();

  // Don't simplify the token factor if the node itself has too many operands.
  if (N->getNumOperands() > TokenFactorInlineLimit)
    return SDValue();

  // If the sole user is a token factor, we should make sure we have a
  // chance to merge them together. This prevents TF chains from inhibiting
  // optimizations.
  if (N->hasOneUse() && N->use_begin()->getOpcode() == ISD::TokenFactor)
    AddToWorklist(*(N->use_begin()));

  SmallVector<SDNode *, 8> TFs;     // List of token factors to visit.
  SmallVector<SDValue, 8> Ops;      // Ops for replacing token factor.
  SmallPtrSet<SDNode*, 16> SeenOps;
  bool Changed = false;             // If we should replace this token factor.

  // Start out with this token factor.
  TFs.push_back(N);

  // Iterate through token factors.  The TFs grows when new token factors are
  // encountered.
  for (unsigned i = 0; i < TFs.size(); ++i) {
    // Limit number of nodes to inline, to avoid quadratic compile times.
    // We have to add the outstanding Token Factors to Ops, otherwise we might
    // drop Ops from the resulting Token Factors.
    if (Ops.size() > TokenFactorInlineLimit) {
      for (unsigned j = i; j < TFs.size(); j++)
        Ops.emplace_back(TFs[j], 0);
      // Drop unprocessed Token Factors from TFs, so we do not add them to the
      // combiner worklist later.
      TFs.resize(i);
      break;
    }

    SDNode *TF = TFs[i];
    // Check each of the operands.
    for (const SDValue &Op : TF->op_values()) {
      switch (Op.getOpcode()) {
      case ISD::EntryToken:
        // Entry tokens don't need to be added to the list. They are
        // redundant.
        Changed = true;
        break;

      case ISD::TokenFactor:
        if (Op.hasOneUse() && !is_contained(TFs, Op.getNode())) {
          // Queue up for processing.
          TFs.push_back(Op.getNode());
          Changed = true;
          break;
        }
        LLVM_FALLTHROUGH;

      default:
        // Only add if it isn't already in the list.
        if (SeenOps.insert(Op.getNode()).second)
          Ops.push_back(Op);
        else
          Changed = true;
        break;
      }
    }
  }

  // Re-visit inlined Token Factors, to clean them up in case they have been
  // removed. Skip the first Token Factor, as this is the current node.
  for (unsigned i = 1, e = TFs.size(); i < e; i++)
    AddToWorklist(TFs[i]);

  // Remove Nodes that are chained to another node in the list. Do so
  // by walking up chains breath-first stopping when we've seen
  // another operand. In general we must climb to the EntryNode, but we can exit
  // early if we find all remaining work is associated with just one operand as
  // no further pruning is possible.

  // List of nodes to search through and original Ops from which they originate.
  SmallVector<std::pair<SDNode *, unsigned>, 8> Worklist;
  SmallVector<unsigned, 8> OpWorkCount; // Count of work for each Op.
  SmallPtrSet<SDNode *, 16> SeenChains;
  bool DidPruneOps = false;

  unsigned NumLeftToConsider = 0;
  for (const SDValue &Op : Ops) {
    Worklist.push_back(std::make_pair(Op.getNode(), NumLeftToConsider++));
    OpWorkCount.push_back(1);
  }

  auto AddToWorklist = [&](unsigned CurIdx, SDNode *Op, unsigned OpNumber) {
    // If this is an Op, we can remove the op from the list. Remark any
    // search associated with it as from the current OpNumber.
    if (SeenOps.contains(Op)) {
      Changed = true;
      DidPruneOps = true;
      unsigned OrigOpNumber = 0;
      while (OrigOpNumber < Ops.size() && Ops[OrigOpNumber].getNode() != Op)
        OrigOpNumber++;
      assert((OrigOpNumber != Ops.size()) &&
             "expected to find TokenFactor Operand");
      // Re-mark worklist from OrigOpNumber to OpNumber
      for (unsigned i = CurIdx + 1; i < Worklist.size(); ++i) {
        if (Worklist[i].second == OrigOpNumber) {
          Worklist[i].second = OpNumber;
        }
      }
      OpWorkCount[OpNumber] += OpWorkCount[OrigOpNumber];
      OpWorkCount[OrigOpNumber] = 0;
      NumLeftToConsider--;
    }
    // Add if it's a new chain
    if (SeenChains.insert(Op).second) {
      OpWorkCount[OpNumber]++;
      Worklist.push_back(std::make_pair(Op, OpNumber));
    }
  };

  for (unsigned i = 0; i < Worklist.size() && i < 1024; ++i) {
    // We need at least be consider at least 2 Ops to prune.
    if (NumLeftToConsider <= 1)
      break;
    auto CurNode = Worklist[i].first;
    auto CurOpNumber = Worklist[i].second;
    assert((OpWorkCount[CurOpNumber] > 0) &&
           "Node should not appear in worklist");
    switch (CurNode->getOpcode()) {
    case ISD::EntryToken:
      // Hitting EntryToken is the only way for the search to terminate without
      // hitting
      // another operand's search. Prevent us from marking this operand
      // considered.
      NumLeftToConsider++;
      break;
    case ISD::TokenFactor:
      for (const SDValue &Op : CurNode->op_values())
        AddToWorklist(i, Op.getNode(), CurOpNumber);
      break;
    case ISD::LIFETIME_START:
    case ISD::LIFETIME_END:
    case ISD::CopyFromReg:
    case ISD::CopyToReg:
      AddToWorklist(i, CurNode->getOperand(0).getNode(), CurOpNumber);
      break;
    default:
      if (auto *MemNode = dyn_cast<MemSDNode>(CurNode))
        AddToWorklist(i, MemNode->getChain().getNode(), CurOpNumber);
      break;
    }
    OpWorkCount[CurOpNumber]--;
    if (OpWorkCount[CurOpNumber] == 0)
      NumLeftToConsider--;
  }

  // If we've changed things around then replace token factor.
  if (Changed) {
    SDValue Result;
    if (Ops.empty()) {
      // The entry token is the only possible outcome.
      Result = DAG.getEntryNode();
    } else {
      if (DidPruneOps) {
        SmallVector<SDValue, 8> PrunedOps;
        //
        for (const SDValue &Op : Ops) {
          if (SeenChains.count(Op.getNode()) == 0)
            PrunedOps.push_back(Op);
        }
        Result = DAG.getTokenFactor(SDLoc(N), PrunedOps);
      } else {
        Result = DAG.getTokenFactor(SDLoc(N), Ops);
      }
    }
    return Result;
  }
  return SDValue();
}
```
