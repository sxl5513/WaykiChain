// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2017-2019 The WaykiChain Developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.


#ifndef ACCOUNT_REGISTER_H
#define ACCOUNT_REGISTER_H

#include "tx.h"

class CAccountRegisterTx : public CBaseTx {
public:
    mutable CUserID minerUid;  // miner pubkey

public:
    CAccountRegisterTx(const CBaseTx *pBaseTx): CBaseTx(ACCOUNT_REGISTER_TX) {
        assert(ACCOUNT_REGISTER_TX == pBaseTx->nTxType);
        *this = *(CAccountRegisterTx *)pBaseTx;
    }
    CAccountRegisterTx(const CUserID &txUidIn, const CUserID &minerUidIn, int64_t feeIn, int validHeightIn) :
        CBaseTx(ACCOUNT_REGISTER_TX, txUidIn, validHeightIn, feeIn) {
        minerUid    = minerUidIn;
    }
    CAccountRegisterTx(): CBaseTx(ACCOUNT_REGISTER_TX) {}

    ~CAccountRegisterTx() {}

    IMPLEMENT_SERIALIZE(
        READWRITE(VARINT(this->nVersion));
        nVersion = this->nVersion;
        READWRITE(VARINT(nValidHeight));
        READWRITE(txUid);

        READWRITE(minerUid);
        READWRITE(VARINT(llFees));
        READWRITE(signature);)

    uint64_t GetFee() const { return llFees; }
    uint64_t GetValue() const { return 0; }

    uint256 ComputeSignatureHash(bool recalculate = false) const {
        if (recalculate || sigHash.IsNull()) {
            assert(txUid.type() == typeid(CPubKey) &&
                   (minerUid.type() == typeid(CPubKey) || minerUid.type() == typeid(CNullID)));

            CHashWriter ss(SER_GETHASH, 0);
            ss << VARINT(nVersion) << nTxType << VARINT(nValidHeight) << txUid << minerUid << VARINT(llFees);

            sigHash = ss.GetHash();
        }

        return sigHash;
    }
    std::shared_ptr<CBaseTx> GetNewInstance() { return std::make_shared<CAccountRegisterTx>(this); }
    bool GetInvolvedKeyIds(set<CKeyID> &vAddr, CAccountViewCache &view, CScriptDBViewCache &scriptDB);
    string ToString(CAccountViewCache &view) const;
    Object ToJson(const CAccountViewCache &AccountView) const;
    bool CheckTx(CValidationState &state, CAccountViewCache &view, CScriptDBViewCache &scriptDB);
    bool ExecuteTx(int nIndex, CAccountViewCache &view, CValidationState &state, CTxUndo &txundo, int nHeight,
                   CTransactionDBCache &txCache, CScriptDBViewCache &scriptDB);
    bool UndoExecuteTx(int nIndex, CAccountViewCache &view, CValidationState &state, CTxUndo &txundo, int nHeight,
                       CTransactionDBCache &txCache, CScriptDBViewCache &scriptDB);
};

#endif