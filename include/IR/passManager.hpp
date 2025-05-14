


/// Manages a sequence of passes over a particular unit of IR.
///
/// A pass manager contains a sequence of passes to run over a particular unit
/// of IR (e.g. Functions, Modules). It is itself a valid pass over that unit of
/// IR, and when run over some given IR will run each of its contained passes in
/// sequence. Pass managers are the primary and most basic building block of a
/// pass pipeline.
///
/// When you run a pass manager, you provide an \c AnalysisManager<IRUnitT>
/// argument. The pass manager will propagate that analysis manager to each
/// pass it runs, and will call the analysis manager's invalidation routine with
/// the PreservedAnalyses of each pass it runs.

class PassManager {
public:
    PassManager();
};
