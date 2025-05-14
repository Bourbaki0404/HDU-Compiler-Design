


/// Manages a sequence of passes over a particular unit of IR.
///
/// A pass manager contains a sequence of passes to run over a particular unit
/// of IR (e.g. Functions, Modules). It is itself a valid pass over that unit of
/// IR, and when run over some given IR will run each of its contained passes in
/// sequence. Pass managers are the primary and most basic building block of a
/// pass pipeline.
class PassManager {
public:
    
};
