#include "stdafx.h"
#include "BMSInfoManager.h"
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;
using namespace std;

//------------------------------------------------------------------------------
BMSInfoManager::BMSInfoManager()
{
    m_otlTime.frac_precision = OTL_CUSTOM_TIME_PRECISION;
}

//------------------------------------------------------------------------------
BMSInfoManager::~BMSInfoManager()
{
}

//------------------------------------------------------------------------------
EErrCode BMSInfoManager::SearchSupplierForTagID(const otl_connect &dbconnect, const uint32_t &lstID, std::shared_ptr<SupplierInfoAndID> &ptResult)
{
    if (!ptResult) return EErrCode::EC_DATABASE_FAIL;
    string devResult;
    if (!SearchDeviceFromID(dbconnect, lstID, devResult))
    {
        return EErrCode::EC_DATABASE_FAIL;
    }
    if (devResult.empty())
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;
    string strSQL = R"(select ��Ӧ�̴���,��Ӧ������,���˴���,��ϵ����,��ϵ�绰,����,��ַ,��������,��������,��ַ,��ע 
from ��Ӧ�� s 
where s.��Ӧ������ = (select �豸��Ӧ�� from �豸 where �豸���� = ')" 
+ devResult + R"('))";

    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        // �ж��Ƿ��н��
        if (!strm.eof())
        {
            ptResult->ulGID = lstID;
            strm >> ptResult->supplierInfo.sSupplierCode;
            strm >> ptResult->supplierInfo.sSupplierName;
            strm >> ptResult->supplierInfo.sConPerson;
            strm >> ptResult->supplierInfo.sConDepartment;
            strm >> ptResult->supplierInfo.sConTelephone;
            strm >> ptResult->supplierInfo.sFax;
            strm >> ptResult->supplierInfo.sAddress;
            strm >> ptResult->supplierInfo.sZipCode;
            strm >> ptResult->supplierInfo.sE_Mail;
            strm >> ptResult->supplierInfo.sURL;
            strm >> ptResult->supplierInfo.sRemarks;
            strm.close();
        }
        else
        {
            LOG_ERROR << "��ѯ�豸��Ӧ����Ϣʧ�ܣ�";
            strm.close();
            return EErrCode::EC_DATABASE_NODATA;
        }
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }

    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode BMSInfoManager::SearchDeviceInfForTagID(const otl_connect &dbconnect, const uint32_t &lstID, std::shared_ptr<DeviceInfoAndID> &ptResult)
{
    if (!ptResult) return EErrCode::EC_DATABASE_FAIL;
    string devResult = "";
    if (!SearchDeviceFromID(dbconnect, lstID, devResult))
    {
        return EErrCode::EC_DATABASE_FAIL;
    }
    if (devResult.empty())
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(select �豸���,�豸����,��װλ��,ʹ������,ʹ�ò���,�豸״��,��������,�豸��Ӧ��,��������,��������,���п�������,���п������� 
from �豸 
where �豸���� = ')"
+ devResult + R"(')";

    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        // �ж��Ƿ��н��
        if (!strm.eof())
        {
            ptResult->ulGID = lstID;
            strm >> ptResult->deviceInfo.DeviceNum;
            strm >> ptResult->deviceInfo.DeviceName;
            strm >> ptResult->deviceInfo.InstallLocation;
            otl_datetime tempTime;
            strm >> tempTime;
            ptResult->deviceInfo.UseDate = TransformTimeOtlTime(tempTime);// to_string(tempTime.year) + " " + to_string(tempTime.month) + " " + to_string(tempTime.day);
            //strm >> ptResult->deviceInfo.UseDate;
            strm >> ptResult->deviceInfo.UsageDepart;
            strm >> ptResult->deviceInfo.DeviceStatus;
            strm >> tempTime;
            ptResult->deviceInfo.ProductionDate = TransformTimeOtlTime(tempTime); //to_string(tempTime.year) + " " + to_string(tempTime.month) + " " + to_string(tempTime.day);
            //strm >> ptResult->deviceInfo.ProductionDate;
            strm >> ptResult->deviceInfo.DeviceSupplier;
            strm >> ptResult->deviceInfo.AlarmUpLimit;
            strm >> ptResult->deviceInfo.AlarmDownLimit;
            strm >> ptResult->deviceInfo.RunCtrlUpLimit;
            strm >> ptResult->deviceInfo.RunCtrlDownLimit;
            strm.close();
        }
        else
        {
            LOG_ERROR << "��ѯ�豸������Ϣʧ�ܣ�";
            strm.close();
            return EErrCode::EC_DATABASE_NODATA;
        }
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }

    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode BMSInfoManager::SearchRepairInfForTagID(const otl_connect &dbconnect, const uint32_t &lstID, std::shared_ptr<RepairInfoAndID> &ptResult)
{
    if (!ptResult) return EErrCode::EC_DATABASE_FAIL;
    string devResult = "";
    if (!SearchDeviceFromID(dbconnect, lstID, devResult))
    {
        return EErrCode::EC_DATABASE_FAIL;
    }
    if (devResult.empty())
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(select �豸����,ά������,ά��ʱ��,����ԭ��,ά�޽��,ά�޾�����,ά�޵�λ,��ע 
from �豸ά�޼�¼ 
where �豸���� = ')"
+ devResult + R"(')";

    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        // �ж��Ƿ��н��
        if (!strm.eof())
        {
            ptResult->ulGID = lstID;
            ptResult->repairInfo.recordingID = -1;
            strm >> ptResult->repairInfo.DeviceName;
            strm >> ptResult->repairInfo.RepairContent;

            otl_datetime tempTime;
            strm >> tempTime;
            ptResult->repairInfo.RepairTime = to_string(tempTime.year) + " " + to_string(tempTime.month) + " " + to_string(tempTime.day);
            //strm >> ptResult->repairInfo.RepairTime;
            strm >> ptResult->repairInfo.FaultReason;
            strm >> ptResult->repairInfo.RepairResults;
            strm >> ptResult->repairInfo.RepairManager;
            strm >> ptResult->repairInfo.RepairUnit;
            strm >> ptResult->repairInfo.Remarks;
            strm.close();
        }
        else
        {
            LOG_ERROR << "δ�鵽ά�޼�¼��";
            strm.close();
            return EErrCode::EC_DATABASE_NODATA;
        }
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }

    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
EErrCode BMSInfoManager::SearchRunInfForTagID(const otl_connect &dbconnect, const uint32_t &lstID, std::shared_ptr<DeviceRunInfoAndID> &ptResult)
{
    if (!ptResult) return EErrCode::EC_DATABASE_FAIL;
    string devResult = "";
    if (!SearchDeviceFromID(dbconnect, lstID, devResult))
    {
        return EErrCode::EC_DATABASE_FAIL;
    }
    if (devResult.empty())
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(select �豸����,��ʼʱ��,����ʱ��,�������,��¼Ա,��ע 
from �豸���м�¼ 
where �豸���� = ')"
+ devResult + R"(')";

    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);
        // �ж��Ƿ��н��
        if (!strm.eof())
        {
            ptResult->ulGID = lstID;
            ptResult->deviceRunInfo.recordingID = -1;
            strm >> ptResult->deviceRunInfo.DeviceName;
            otl_datetime tempTime;
            strm >> tempTime;
            ptResult->deviceRunInfo.StartTime = to_string(tempTime.year) + " " + to_string(tempTime.month) + " " + to_string(tempTime.day);
            //strm >> ptResult->deviceRunInfo.StartTime;
            strm >> tempTime;
            ptResult->deviceRunInfo.EndTime = to_string(tempTime.year) + " " + to_string(tempTime.month) + " " + to_string(tempTime.day);
            strm >> ptResult->deviceRunInfo.EndTime;
            strm >> ptResult->deviceRunInfo.Operation;
            strm >> ptResult->deviceRunInfo.Recorder;
            strm >> ptResult->deviceRunInfo.Remarks;
            strm.close();
        }
        else
        {
            LOG_ERROR << "δ���ҵ��豸���м�¼��";
            strm.close();
            return EErrCode::EC_DATABASE_NODATA;
        }
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }

    return EErrCode::EC_SUCCESS;
}

//------------------------------------------------------------------------------
bool BMSInfoManager::SearchDeviceFromID(const otl_connect &dbconnect, const uint32_t &lstID, string &devResult)
{
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(SELECT �豸���� FROM �豸������ WHERE ������� = )" + to_string(lstID);
    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        if (!strm.eof())
        {
            strm >> devResult;
        }
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return false;
    }
    return true;
}

//------------------------------------------------------------------------------
EErrCode BMSInfoManager::AddSuplier(const otl_connect &dbconnect, const std::shared_ptr<SupplierInfo> &ptResult)
{
    if (NameExists(dbconnect, "��Ӧ��", "��Ӧ������", ptResult->sSupplierName))
    {
        return EErrCode::EC_DATABASE_ALREADYEXISTS;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    /*string strSQL = R"(INSERT INTO ��Ӧ�� (��Ӧ�̴���,��Ӧ������,��ϵ��,��ϵ����,��ϵ�绰,����,��ַ,��������,��������,��ַ,��ע) VALUES ()";
    strSQL += R"(')" + ptResult->sSupplierCode + R"(',)";
    strSQL += R"(')" + ptResult->sSupplierName + R"(',)";
    strSQL += R"(')" + ptResult->sConPerson + R"(',)";
    strSQL += R"(')" + ptResult->sConDepartment + R"(',)";
    strSQL += R"(')" + ptResult->sConTelephone + R"(',)";
    strSQL += R"(')" + ptResult->sFax + R"(',)";
    strSQL += R"(')" + ptResult->sAddress + R"(',)";
    strSQL += R"(')" + ptResult->sZipCode + R"(',)";
    strSQL += R"(')" + ptResult->sE_Mail + R"(',)";
    strSQL += R"(')" + ptResult->sURL + R"(',)";
    strSQL += R"(')" + ptResult->sRemarks + R"('))";*/
    
    string strSQL = R"(INSERT INTO ��Ӧ�� (��Ӧ�̴���,��Ӧ������,��ϵ��,��ϵ����,��ϵ�绰,����,��ַ,��������,��������,��ַ,��ע) VALUES (
:v_firstColumn<char[10]>,
:v_secondColumn<char[40]>,
:v_thirdColumn<char[20]>,
:v_fourthColumn<char[30]>,
:v_fifthColumn<char[15]>,
:v_sixthColumn<char[15]>,
:v_seventhColumn<char[60]>,
:v_eighthColumn<char[6]>,
:v_ninthColumn<char[50]>,
:v_tenthColumn<char[60]>,
:v_eleventhColumn<char[100]>))";

    try
    {
        strm.set_commit(0);
        strm.open(strSQL.size(), strSQL.c_str(), db);
        strm << ptResult->sSupplierCode;
        strm << ptResult->sSupplierName;
        strm << ptResult->sConPerson;
        strm << ptResult->sConDepartment;
        strm << ptResult->sConTelephone;
        strm << ptResult->sFax;
        strm << ptResult->sAddress;
        strm << ptResult->sZipCode;
        strm << ptResult->sE_Mail;
        strm << ptResult->sURL;
        strm << ptResult->sRemarks;

        strm.flush();
        // �ύ����
        db.commit();
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::DeleteSupplier(const otl_connect &dbconnect, const string supplierName)
{
    if (!NameExists(dbconnect, "��Ӧ��", "��Ӧ������", supplierName))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }

    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(DELETE FROM ��Ӧ�� WHERE ��Ӧ������ = ')" + supplierName + R"(')";

    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        strm.flush();
        strm.close();
        // �ύ����
        db.commit();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::ModifySuplier(const otl_connect &dbconnect, const std::shared_ptr<SupplierInfo> &ptResult)
{
    if (!NameExists(dbconnect, "��Ӧ��", "��Ӧ������", ptResult->sSupplierName))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(UPDATE ��Ӧ�� SET )"; 
    strSQL += R"(��Ӧ�̴��� = ')" + ptResult->sSupplierCode + R"(',)";
    strSQL += R"(��ϵ�� = ')" + ptResult->sConPerson + R"(',)";
    strSQL += R"(��ϵ���� = ')" + ptResult->sConDepartment + R"(',)";
    strSQL += R"(��ϵ�绰 = ')" + ptResult->sConTelephone + R"(',)";
    strSQL += R"(���� = ')" + ptResult->sFax + R"(',)";
    strSQL += R"(��ַ = ')" + ptResult->sAddress + R"(',)";
    strSQL += R"(�������� = ')" + ptResult->sZipCode + R"(',)";
    strSQL += R"(�������� = ')" + ptResult->sE_Mail + R"(',)";
    strSQL += R"(��ַ = ')" + ptResult->sURL + R"(',)";
    strSQL += R"(��ע = ')" + ptResult->sRemarks + R"(')";
    strSQL += R"( WHERE ��Ӧ������ = ')" + ptResult->sSupplierName + R"(')";

    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        strm.flush();
        strm.close();
        // �ύ����
        db.commit();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::SearchSuplier(const otl_connect &dbconnect, const string supplierName, list<std::shared_ptr<SupplierInfo>> &ptResult)
{
    if (!NameExists(dbconnect, "��Ӧ��", "��Ӧ������", supplierName))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(SELECT  ��Ӧ�̴���,��Ӧ������,��ϵ��,��ϵ����,��ϵ�绰,����,��ַ,��������,��������,��ַ,��ע FROM ��Ӧ��)";
    if (!supplierName.empty())
    {
        strSQL += R"( WHERE ��Ӧ������ = ')" + supplierName + R"(')";
    }
    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        // �ж��Ƿ��н��
        while (!strm.eof())
        {
            std::shared_ptr<SupplierInfo> tempSupplier = make_shared<SupplierInfo>();
            strm >> tempSupplier->sSupplierCode;
            strm >> tempSupplier->sSupplierName;
            strm >> tempSupplier->sConPerson;
            strm >> tempSupplier->sConDepartment;
            strm >> tempSupplier->sConTelephone;
            strm >> tempSupplier->sFax;
            strm >> tempSupplier->sAddress;
            strm >> tempSupplier->sZipCode;
            strm >> tempSupplier->sE_Mail;
            strm >> tempSupplier->sURL;
            strm >> tempSupplier->sRemarks;
            ptResult.push_back(tempSupplier);
        }
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::AddMeterType(const otl_connect &dbconnect, const std::shared_ptr<MeterType> &ptResult)
{
    if (NameExists(dbconnect, "����������", "����������", ptResult->strMeterType))
    {
        return EErrCode::EC_DATABASE_ALREADYEXISTS;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(INSERT INTO ���������� (����������,����˵��) VALUES (
:v_firstColumn<char[24]>,
:v_secondColumn<char[200]>))";

    try
    {
        strm.set_commit(0);
        strm.open(strSQL.size(), strSQL.c_str(), db);
        strm << ptResult->strMeterType;
        strm << ptResult->TypeDes;

        strm.flush();
        // �ύ����
        db.commit();
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::DeleteMeterType(const otl_connect &dbconnect, const string meterTypeName)
{
    if (!NameExists(dbconnect, "����������", "����������", meterTypeName))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }

    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(DELETE FROM ���������� WHERE ���������� = ')" + meterTypeName + R"(')";

    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        strm.flush();
        strm.close();
        // �ύ����
        db.commit();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::ModifyMeterType(const otl_connect &dbconnect, const std::shared_ptr<MeterType> &ptResult)
{
    if (!NameExists(dbconnect, "����������", "����������", ptResult->strMeterType))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(UPDATE ���������� SET )";
    strSQL += R"(����˵�� = ')" + ptResult->TypeDes + R"(')";
    strSQL += R"( WHERE ���������� = ')" + ptResult->strMeterType + R"(')";

    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        strm.flush();
        strm.close();
        // �ύ����
        db.commit();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::SearchMeterType(const otl_connect &dbconnect, const string meterTypeName, list<std::shared_ptr<MeterType>> &ptResult)
{
    if (!NameExists(dbconnect, "����������", "����������", meterTypeName))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(SELECT  ����������,����˵�� FROM ����������)";
    if (!meterTypeName.empty())
    {
        strSQL += R"( WHERE ���������� = ')" + meterTypeName + R"(')";
    }
    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        // �ж��Ƿ��н��
        while (!strm.eof())
        {
            std::shared_ptr<MeterType> tempMeterType = make_shared<MeterType>();
            strm >> tempMeterType->strMeterType;
            strm >> tempMeterType->TypeDes;
            ptResult.push_back(tempMeterType);
        }
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::AddEnergyCate(const otl_connect &dbconnect, const std::shared_ptr<EnergyCate> &ptResult)
{
    if (NameExists(dbconnect, "�������", "�������", ptResult->strEnergyCate))
    {
        return EErrCode::EC_DATABASE_ALREADYEXISTS;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(INSERT INTO ������� (�������,���˵��) VALUES (
:v_firstColumn<char[30]>,
:v_secondColumn<char[200]>))";

    try
    {
        strm.set_commit(0);
        strm.open(strSQL.size(), strSQL.c_str(), db);
        strm << ptResult->strEnergyCate;
        strm << ptResult->TypeDes;

        strm.flush();
        // �ύ����
        db.commit();
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::DeleteEnergyCate(const otl_connect &dbconnect, const string name)
{
    if (!NameExists(dbconnect, "�������", "�������", name))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }

    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(DELETE FROM ������� WHERE ������� = ')" + name + R"(')";

    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        strm.flush();
        strm.close();
        // �ύ����
        db.commit();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::ModifyEnergyCate(const otl_connect &dbconnect, const std::shared_ptr<EnergyCate> &ptResult)
{
    if (!NameExists(dbconnect, "�������", "�������", ptResult->strEnergyCate))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(UPDATE ������� SET )";
    strSQL += R"(���˵�� = ')" + ptResult->TypeDes + R"(')";
    strSQL += R"( WHERE ������� = ')" + ptResult->strEnergyCate + R"(')";

    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        strm.flush();
        strm.close();
        // �ύ����
        db.commit();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::SearchEnergyCate(const otl_connect &dbconnect, const string name, list<std::shared_ptr<EnergyCate>> &ptResult)
{
    if (!NameExists(dbconnect, "�������", "�������", name))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(SELECT  �������,���˵�� FROM �������)";
    if (!name.empty())
    {
        strSQL += R"( WHERE ������� = ')" + name + R"(')";
    }
    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        // �ж��Ƿ��н��
        while (!strm.eof())
        {
            std::shared_ptr<EnergyCate> tempEnergyCate = make_shared<EnergyCate>();
            strm >> tempEnergyCate->strEnergyCate;
            strm >> tempEnergyCate->TypeDes;
            ptResult.push_back(tempEnergyCate);
        }
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::AddDevFaultCate(const otl_connect &dbconnect, const std::shared_ptr<DevFaultCate> &ptResult)
{
    if (NameExists(dbconnect, "�������", "�������", ptResult->FaultCate))
    {
        return EErrCode::EC_DATABASE_ALREADYEXISTS;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(INSERT INTO ������� (�������,���˵��,��������) VALUES (
:v_firstColumn<char[30]>,
:v_secondColumn<char[200]>,
:v_thirdColumn<char[200]>))";

    try
    {
        strm.set_commit(0);
        strm.open(strSQL.size(), strSQL.c_str(), db);
        strm << ptResult->FaultCate;
        strm << ptResult->CategoryDes;
        strm << ptResult->FaultFeature;

        strm.flush();
        // �ύ����
        db.commit();
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::DeleteDevFaultCate(const otl_connect &dbconnect, const string name)
{
    if (!NameExists(dbconnect, "�������", "�������", name))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }

    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(DELETE FROM ������� WHERE ������� = ')" + name + R"(')";

    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        strm.flush();
        strm.close();
        // �ύ����
        db.commit();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::ModifyDevFaultCate(const otl_connect &dbconnect, const std::shared_ptr<DevFaultCate> &ptResult)
{
    if (!NameExists(dbconnect, "�������", "�������", ptResult->FaultCate))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(UPDATE ������� SET )";
    strSQL += R"(���˵�� = ')" + ptResult->CategoryDes + R"(')";
    strSQL += R"(, �������� = ')" + ptResult->FaultFeature + R"(')";
    strSQL += R"( WHERE ������� = ')" + ptResult->FaultCate + R"(')";

    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        strm.flush();
        strm.close();
        // �ύ����
        db.commit();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::SearchDevFaultCate(const otl_connect &dbconnect, const string name, list<std::shared_ptr<DevFaultCate>> &ptResult)
{
    if (!NameExists(dbconnect, "�������", "�������", name))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(SELECT  �������,���˵��,�������� FROM �������)";
    if (!name.empty())
    {
        strSQL += R"( WHERE ������� = ')" + name + R"(')";
    }
    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        // �ж��Ƿ��н��
        while (!strm.eof())
        {
            std::shared_ptr<DevFaultCate> tempDevFaultCate = make_shared<DevFaultCate>();
            strm >> tempDevFaultCate->FaultCate;
            strm >> tempDevFaultCate->CategoryDes;
            strm >> tempDevFaultCate->FaultFeature;
            ptResult.push_back(tempDevFaultCate);
        }
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::AddDeviceInfo(const otl_connect &dbconnect, const std::shared_ptr<DeviceInfo> &ptResult)
{
    if (NameExists(dbconnect, "�豸", "�豸����", ptResult->DeviceName))
    {
        return EErrCode::EC_DATABASE_ALREADYEXISTS;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(INSERT INTO �豸 (�豸���,�豸����,��װλ��,ʹ������,ʹ�ò���,�豸״��,��������,�豸��Ӧ��,��������,��������,���п�������,���п�������) VALUES (
:v_firstColumn<char[12]>,
:v_secondColumn<char[24]>,
:v_thirdColumn<char[60]>,
:v_fourthColumn<timestamp>,
:v_fifthColumn<char[40]>,
:v_sixthColumn<char[50]>,
:v_seventhColumn<timestamp>,
:v_eighthColumn<char[60]>,
:v_ninthColumn<double>,
:v_tenthColumn<double>,
:v_eleventhColumn<double>,
:v_twelfthColumn<double>))";

    try
    {
        strm.set_commit(0);
        strm.open(strSQL.size(), strSQL.c_str(), db);
        strm << ptResult->DeviceNum;
        strm << ptResult->DeviceName;
        strm << ptResult->InstallLocation;
        strm << TransformTimeString(ptResult->UseDate);
        strm << ptResult->UsageDepart;
        strm << ptResult->DeviceStatus;
        strm << TransformTimeString(ptResult->ProductionDate);
        strm << ptResult->DeviceSupplier;
        strm << ptResult->AlarmUpLimit;
        strm << ptResult->AlarmDownLimit;
        strm << ptResult->RunCtrlUpLimit;
        strm << ptResult->RunCtrlDownLimit;

        strm.flush();
        // �ύ����
        db.commit();
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::DeleteDeviceInfo(const otl_connect &dbconnect, const string name)
{
    if (!NameExists(dbconnect, "�豸", "�豸����", name))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }

    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(DELETE FROM �豸 WHERE �豸���� = ')" + name + R"(')";

    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        strm.flush();
        strm.close();
        // �ύ����
        db.commit();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::ModifyDeviceInfo(const otl_connect &dbconnect, const std::shared_ptr<DeviceInfo> &ptResult)
{
    if (!NameExists(dbconnect, "�豸", "�豸����", ptResult->DeviceName))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;
    /*string strSQL = R"(UPDATE �豸 SET )";
    strSQL += R"(�豸��� = ')" + ptResult->DeviceNum + R"(')";
    strSQL += R"(, ��װλ�� = ')" + ptResult->InstallLocation + R"(')";
    strSQL += R"(, ʹ������ = ')" + ptResult->UseDate + R"(')";
    strSQL += R"(, ʹ�ò��� = ')" + ptResult->UsageDepart + R"(')";
    strSQL += R"(, �豸״�� = ')" + ptResult->DeviceStatus + R"(')";
    strSQL += R"(, �������� = ')" + ptResult->ProductionDate + R"(')";
    strSQL += R"(, �豸��Ӧ�� = ')" + ptResult->DeviceSupplier + R"(')";
    strSQL += R"(, �������� = ')" + to_string(ptResult->AlarmUpLimit) + R"(')";
    strSQL += R"(, �������� = ')" + to_string(ptResult->AlarmDownLimit) + R"(')";
    strSQL += R"(, ���п������� = ')" + to_string(ptResult->RunCtrlUpLimit) + R"(')";
    strSQL += R"(, ���п������� = ')" + to_string(ptResult->RunCtrlDownLimit) + R"(')";
    strSQL += R"( WHERE �豸���� = ')" + ptResult->DeviceName + R"(')";*/
    string strSQL = R"(UPDATE �豸
        SET
        �豸��� = :v_firstColumn<char[12]>,
        ��װλ�� = :v_thirdColumn<char[60]>,
        ʹ������ = :v_fourthColumn<timestamp>,
        ʹ�ò��� = :v_fifthColumn<char[40]>,
        �豸״�� = :v_sixthColumn<char[50]>,
        �������� = :v_seventhColumn<timestamp>,
        �豸��Ӧ�� = :v_eighthColumn<char[60]>,
        �������� = :v_ninthColumn<double>,
        �������� = :v_tenthColumn<double>,
        ���п������� = :v_eleventhColumn<double>,
        ���п������� = :v_twelfthColumn<double>
        WHERE �豸���� = :v_secondColumn<char[24]>)";

    try
    {
        strm.set_commit(0);
        strm.open(strSQL.size(), strSQL.c_str(), db);

        strm << ptResult->DeviceNum;
        strm << ptResult->InstallLocation;
        strm << TransformTimeString(ptResult->UseDate);
        strm << ptResult->UsageDepart;
        strm << ptResult->DeviceStatus;
        strm << TransformTimeString(ptResult->ProductionDate);
        strm << ptResult->DeviceSupplier;
        strm << ptResult->AlarmUpLimit;
        strm << ptResult->AlarmDownLimit;
        strm << ptResult->RunCtrlUpLimit;
        strm << ptResult->RunCtrlDownLimit;
        strm << ptResult->DeviceName;

        strm.flush();
        strm.close();
        // �ύ����
        db.commit();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::SearchDeviceInfo(const otl_connect &dbconnect, const string name, list<std::shared_ptr<DeviceInfo>> &ptResult)
{
    if (!NameExists(dbconnect, "�豸", "�豸����", name))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(SELECT  �豸���,�豸����,��װλ��,ʹ������,ʹ�ò���,�豸״��,��������,�豸��Ӧ��,��������,��������,���п�������,���п������� FROM �豸)";
    if (!name.empty())
    {
        strSQL += R"( WHERE �豸���� = ')" + name + R"(')";
    }
    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        // �ж��Ƿ��н��
        while (!strm.eof())
        {
            std::shared_ptr<DeviceInfo> tempDeviceInfo = make_shared<DeviceInfo>();
            strm >> tempDeviceInfo->DeviceNum;
            strm >> tempDeviceInfo->DeviceName;
            strm >> tempDeviceInfo->InstallLocation;
            otl_datetime otlTime;
            strm >> otlTime;
            tempDeviceInfo->UseDate = TransformTimeOtlTime(otlTime);
            strm >> tempDeviceInfo->UsageDepart;
            strm >> tempDeviceInfo->DeviceStatus;
            strm >> otlTime;
            tempDeviceInfo->ProductionDate = TransformTimeOtlTime(otlTime);
            strm >> tempDeviceInfo->DeviceSupplier;
            strm >> tempDeviceInfo->AlarmUpLimit;
            strm >> tempDeviceInfo->AlarmDownLimit;
            strm >> tempDeviceInfo->RunCtrlUpLimit;
            strm >> tempDeviceInfo->RunCtrlDownLimit;
            ptResult.push_back(tempDeviceInfo);
        }
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::AddMeterInfo(const otl_connect &dbconnect, const std::shared_ptr<MeterInfo> &ptResult)
{
    if (NameExists(dbconnect, "������", "��������", ptResult->MeterNumber))
    {
        return EErrCode::EC_DATABASE_ALREADYEXISTS;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(INSERT INTO ������ (��������,����������,�������,���ͺ�,ʹ�õ�λ,���ʱ�׼,��׼������λ,��װ����,��װλ��,��������,����,ת�����,��ĳ�ʼ����,ת�����,��ע) VALUES (
:v_firstColumn<char[12]>,
:v_secondColumn<char[24]>,
:v_thirdColumn<char[30]>,
:v_fourthColumn<char[20]>,
:v_fifthColumn<char[40]>,
:v_sixthColumn<double>,
:v_seventhColumn<char[8]>,
:v_eighthColumn<timestamp>,
:v_ninthColumn<char[40]>,
:v_tenthColumn<char[40]>,
:v_eleventhColumn<short>,
:v_twelfthColumn<double>,
:v_thirteenthColumn<double>,
:v_fourteenthColumn<double>,
:v_fifteenthColumn<char[200]>))";
//:v_sixteenthColumn<double>))";//�������

    try
    {
        strm.set_commit(0);
        strm.open(strSQL.size(), strSQL.c_str(), db);

        strm << ptResult->MeterNumber;
        strm << ptResult->strMeterType;
        strm << ptResult->strEnergyCate;
        strm << ptResult->MeterModel;
        strm << ptResult->UseUnits;
        strm << ptResult->RateStandard;
        strm << ptResult->StandardUnit;
        strm << TransformTimeString(ptResult->InstallDate);
        strm << ptResult->InstallLocation;
        strm << ptResult->ProductionFactory;
        strm << ptResult->MeterMagnification;
        strm << ptResult->TranMeterBase;
        strm << ptResult->MeterInitValue;
        strm << ptResult->TranMeterValue;
        strm << ptResult->Remarks;
        //strm << ptResult->OldMeterNumber;

        strm.flush();
        // �ύ����
        db.commit();
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::DeleteMeterInfo(const otl_connect &dbconnect, const string name)
{
    if (!NameExists(dbconnect, "������", "��������", name))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }

    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(DELETE FROM ������ WHERE �������� = ')" + name + R"(')";

    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        strm.flush();
        strm.close();
        // �ύ����
        db.commit();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::ModifyMeterInfo(const otl_connect &dbconnect, const std::shared_ptr<MeterInfo> &ptResult)
{
    if (!NameExists(dbconnect, "������", "��������", ptResult->MeterNumber))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(UPDATE ������
        SET
        ���������� = :v_secondColumn<char[24]>,
        ������� = :v_thirdColumn<char[30]>,
        ���ͺ� = :v_fourthColumn<char[20]>,
        ʹ�õ�λ = :v_fifthColumn<char[40]>,
        ���ʱ�׼ = :v_sixthColumn<double>,
        ��׼������λ = :v_seventhColumn<char[8]>,
        ��װ���� = :v_eighthColumn<timestamp>,
        ��װλ�� = :v_ninthColumn<char[40]>,
        �������� = :v_tenthColumn<char[40]>,
        ���� = :v_eleventhColumn<short>,
        ת����� = :v_twelfthColumn<double>,
        ��ĳ�ʼ���� = :v_thirteenthColumn<double>,
        ת����� = :v_fourteenthColumn<double>,
        ��ע = :v_fifteenthColumn<char[200]>
        WHERE �������� = :v_firstColumn<char[12]>)";
    try
    {
        strm.set_commit(0);
        strm.open(strSQL.size(), strSQL.c_str(), db);

        strm << ptResult->strMeterType;
        strm << ptResult->strEnergyCate;
        strm << ptResult->MeterModel;
        strm << ptResult->UseUnits;
        strm << ptResult->RateStandard;
        strm << ptResult->StandardUnit;
        strm << TransformTimeString(ptResult->InstallDate);
        strm << ptResult->InstallLocation;
        strm << ptResult->ProductionFactory;
        strm << ptResult->MeterMagnification;
        strm << ptResult->TranMeterBase;
        strm << ptResult->MeterInitValue;
        strm << ptResult->TranMeterValue;
        strm << ptResult->Remarks;
        //strm << ptResult->OldMeterNumber;
        strm << ptResult->MeterNumber;
        strm.flush();
        strm.close();
        // �ύ����
        db.commit();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::SearchMeterInfo(const otl_connect &dbconnect, const string name, list<std::shared_ptr<MeterInfo>> &ptResult)
{
    if (!NameExists(dbconnect, "������", "��������", name))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(SELECT ��������,����������,�������,���ͺ�,ʹ�õ�λ,���ʱ�׼,��׼������λ,��װ����,��װλ��,��������,����,ת�����,��ĳ�ʼ����,ת�����,��ע,�ɼ�������� FROM ������)";
    if (!name.empty())
    {
        strSQL += R"( WHERE �������� = ')" + name + R"(')";
    }
    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        // �ж��Ƿ��н��
        while (!strm.eof())
        {
            std::shared_ptr<MeterInfo> tempMeterInfo = make_shared<MeterInfo>();
            strm >> tempMeterInfo->MeterNumber;
            strm >> tempMeterInfo->strMeterType;
            strm >> tempMeterInfo->strEnergyCate;
            strm >> tempMeterInfo->MeterModel;
            strm >> tempMeterInfo->UseUnits;
            strm >> tempMeterInfo->RateStandard;
            strm >> tempMeterInfo->StandardUnit;
            otl_datetime otlTime;
            strm >> otlTime;
            tempMeterInfo->InstallDate = TransformTimeOtlTime(otlTime);
            strm >> tempMeterInfo->InstallLocation;
            strm >> tempMeterInfo->ProductionFactory;
            strm >> tempMeterInfo->MeterMagnification;
            strm >> tempMeterInfo->TranMeterBase;
            strm >> tempMeterInfo->MeterInitValue;
            strm >> tempMeterInfo->TranMeterValue;
            strm >> tempMeterInfo->Remarks;
            strm >> tempMeterInfo->tagID;
            //strm >> tempMeterInfo->OldMeterNumber;
            ptResult.push_back(tempMeterInfo);
        }
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::AddDeviceParameter(const otl_connect &dbconnect, const std::shared_ptr<DeviceParameter> &ptResult)
{
    for (auto &item : ptResult->lstGID)
    {
        if (IdExists(dbconnect, "�豸������", "�������", item.first))
        {
            //ptResult->lstGID.erase(item);
            return EErrCode::EC_DATABASE_ALREADYEXISTS;
        }
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(INSERT INTO �豸������ (�豸����,�������,��������) VALUES (
:v_firstColumn<char[12]>,
:v_secondColumn<int>,
:v_thirdColumn<char[40]>))";

    try
    {
        strm.set_commit(0);
        strm.open(strSQL.size(), strSQL.c_str(), db);
        for (auto &item : ptResult->lstGID)
        {
            strm << ptResult->DeviceName;
            strm << static_cast<int>(item.first);
            strm << item.second;
        }
        strm.flush();
        // �ύ����
        db.commit();
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::DeleteDeviceParameter(const otl_connect &dbconnect, const std::shared_ptr<DeviceParameter> &ptResult)
{
    for (auto &item : ptResult->lstGID)
    {
        if (!IdExists(dbconnect, "�豸������", "�������", item.first))
        {
            return EErrCode::EC_DATABASE_NODATA;
        }
    }

    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(
DELETE FROM �豸������
WHERE ������� = :v_v_firstColumn<int>
)";
    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);
        for (auto &itemID : ptResult->lstGID)
            strm << static_cast<int>(itemID.first);
        strm.flush();
        strm.close();
        // �ύ����
        db.commit();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::SearchDeviceParameter(const otl_connect &dbconnect, const string name, std::shared_ptr<DeviceParameter> &ptResult)
{
    if (name.empty())
    {
        return EErrCode::EC_DATABASE_FAIL;
    }
    if (!NameExists(dbconnect, "�豸������", "�豸����", name))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(SELECT �������,�������� FROM �豸������)";
    strSQL += R"( WHERE �豸���� = ')" + name + R"(')";

    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        if (strm.eof())
        {
            strm.close();
            return EErrCode::EC_DATABASE_NODATA;
        }
        // �ж��Ƿ��н��
        while (!strm.eof())
        {
            uint32_t id = 0;
            string tempTagName = "";
            strm >> id;
            strm >> tempTagName;
            ptResult->lstGID.insert(map<uint32_t, string>::value_type(id, tempTagName));
        }
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    ptResult->DeviceName = name;
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::AddMeterParameter(const otl_connect &dbconnect, const std::shared_ptr<MeterParameter> &ptResult)
{
    if (!NameExists(dbconnect, "������", "��������", ptResult->MeterNum))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }

    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(UPDATE ������
        SET
        �ɼ�������� = :v_tagID<int>
        WHERE �������� = :v_meterNumber<char[12]>)";

    try
    {
        strm.set_commit(0);
        strm.open(strSQL.size(), strSQL.c_str(), db);
        strm << static_cast<int>(ptResult->ulGID);
        strm << ptResult->MeterNum;
        strm.flush();
        // �ύ����
        db.commit();
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::DeleteMeterParameter(const otl_connect &dbconnect, const std::shared_ptr<MeterParameter> &ptResult)
{
    if (!IdExists(dbconnect, "������", "�ɼ��������", ptResult->ulGID))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }

    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    /*string strSQL = R"(
DELETE �ɼ�������� FROM ������
WHERE �������� = :v_v_firstColumn<char[12]>
)";*/
    string strSQL = R"(UPDATE ������
        SET
        �ɼ�������� = :v_tagID<int>
        WHERE �������� = :v_meterNumber<char[12]>)";
    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);
        strm << 0;
        strm << ptResult->MeterNum;
        strm.flush();
        strm.close();
        // �ύ����
        db.commit();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::SearchMeterParameter(const otl_connect &dbconnect, const string name, std::shared_ptr<MeterParameter> &ptResult)
{
    if (name.empty())
    {
        return EErrCode::EC_DATABASE_FAIL;
    }
    if (!NameExists(dbconnect, "������", "��������", name))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(SELECT �ɼ�������� FROM ������)";
    strSQL += R"( WHERE �������� = ')" + name + R"(')";

    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        // �ж��Ƿ��н��
        if (!strm.eof())
        {
            uint32_t id = 0;
            strm >> id;
            ptResult->ulGID = id;
            strm.close();
        }
        else
        {
            return EErrCode::EC_DATABASE_NODATA;
            strm.close();
        }
        
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    ptResult->MeterNum = name;
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::AddDeviceRepair(const otl_connect &dbconnect, const std::shared_ptr<RepairInfo> &ptResult, uint32_t & ulRecordingID)
{
    /*if (NameExists(dbconnect, "�豸", "�豸����", ptResult->DeviceName))
    {
        return EErrCode::EC_DATABASE_ALREADYEXISTS;
    }*/
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(INSERT INTO �豸ά�޼�¼ (�豸����,ά������,ά��ʱ��,����ԭ��,ά�޽��,ά�޾�����,ά�޵�λ,��ע) OUTPUT INSERTED.��¼id VALUES (
:v_firstColumn<char[12]>,
:v_secondColumn<char[40]>,
:v_thirdColumn<timestamp>,
:v_fourthColumn<char[200]>,
:v_fifthColumn<char[200]>,
:v_sixthColumn<char[12]>,
:v_seventhColumn<char[40]>,
:v_eighthColumn<char[200]>))";

    try
    {
        strm.set_commit(0);
        strm.open(strSQL.size(), strSQL.c_str(), db, otl_implicit_select);

        strm << ptResult->DeviceName;
        strm << ptResult->RepairContent;
        strm << TransformTimeString(ptResult->RepairTime);
        strm << ptResult->FaultReason;
        strm << ptResult->RepairResults;
        strm << ptResult->RepairManager;
        strm << ptResult->RepairUnit;
        strm << ptResult->Remarks;

        // û�з���ID���׳��쳣
        if (strm.eof()) throw otl_exception(OTL_CUSTOM_EXCEPTION_IDENTITY_INVALID, 0);

        // ��ȡ�����ID
        strm >> ulRecordingID;

        strm.flush();
        // �ύ����
        db.commit();
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::DeleteDeviceRepair(const otl_connect &dbconnect, const int name)
{
    if (!IdExists(dbconnect, "�豸ά�޼�¼", "��¼ID", name))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }

    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(DELETE FROM �豸ά�޼�¼ WHERE ��¼ID = ')" + to_string(name) + R"(')";

    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        strm.flush();
        strm.close();
        // �ύ����
        db.commit();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::ModifyDeviceRepair(const otl_connect &dbconnect, const std::shared_ptr<RepairInfo> &ptResult)
{
    if (!IdExists(dbconnect, "�豸ά�޼�¼", "��¼ID", ptResult->recordingID))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;
    string strSQL = R"(UPDATE �豸ά�޼�¼
        SET
        �豸���� = :v_firstColumn<char[12]>,
        ά������ = :v_secondColumn<char[40]>,
        ά��ʱ�� = :v_thirdColumn<timestamp>,
        ����ԭ�� = :v_fourthColumn<char[200]>,
        ά�޽�� = :v_fifthColumn<char[200]>,
        ά�޾����� = :v_sixthColumn<char[12]>,
        ά�޵�λ = :v_seventhColumn<char[40]>,
        ��ע = :v_eighthColumn<char[200]>
        WHERE ��¼ID = :v_ID<int>)";
    
    try
    {
        strm.set_commit(0);
        strm.open(strSQL.size(), strSQL.c_str(), db);

        strm << ptResult->DeviceName;
        strm << ptResult->RepairContent;
        strm << TransformTimeString(ptResult->RepairTime);
        strm << ptResult->FaultReason;
        strm << ptResult->RepairResults;
        strm << ptResult->RepairManager;
        strm << ptResult->RepairUnit;
        strm << ptResult->Remarks;
        strm << ptResult->recordingID;
        strm.flush();
        strm.close();
        // �ύ����
        db.commit();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::SearchDeviceRepair(const otl_connect &dbconnect, const int name, list<shared_ptr<RepairInfo>> &ptResult)
{
    if (-1 != name && !IdExists(dbconnect, "�豸ά�޼�¼", "��¼ID", name))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(SELECT  ��¼ID,�豸����,ά������,ά��ʱ��,����ԭ��,ά�޽��,ά�޾�����,ά�޵�λ,��ע FROM �豸ά�޼�¼)";
    if (-1 != name)
    {
        strSQL += R"( WHERE ��¼ID = ')" + to_string(name) + R"(')";
    }
    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        // �ж��Ƿ��н��
        while (!strm.eof())
        {
            std::shared_ptr<RepairInfo> tempDevicRepair = make_shared<RepairInfo>();
            strm >> tempDevicRepair->recordingID;
            strm >> tempDevicRepair->DeviceName;
            strm >> tempDevicRepair->RepairContent;
            otl_datetime otlTime;
            strm >> otlTime;
            tempDevicRepair->RepairTime = TransformTimeOtlTime(otlTime);
            strm >> tempDevicRepair->FaultReason;
            strm >> tempDevicRepair->RepairResults;
            strm >> tempDevicRepair->RepairManager;
            strm >> tempDevicRepair->RepairUnit;
            strm >> tempDevicRepair->Remarks;
            
            ptResult.push_back(tempDevicRepair);
        }
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::SearchDeviceRepair(const otl_connect &dbconnect, const string name, list<shared_ptr<RepairInfo>> &ptResult)
{
    if (!NameExists(dbconnect, "�豸ά�޼�¼", "�豸����", name))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(SELECT  ��¼ID,�豸����,ά������,ά��ʱ��,����ԭ��,ά�޽��,ά�޾�����,ά�޵�λ,��ע FROM �豸ά�޼�¼)";
    if (!name.empty())
    {
        strSQL += R"( WHERE �豸���� = ')" + name + R"(')";
    }
    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        // �ж��Ƿ��н��
        while (!strm.eof())
        {
            std::shared_ptr<RepairInfo> tempDevicRepair = make_shared<RepairInfo>();
            strm >> tempDevicRepair->recordingID;
            strm >> tempDevicRepair->DeviceName;
            strm >> tempDevicRepair->RepairContent;
            otl_datetime otlTime;
            strm >> otlTime;
            tempDevicRepair->RepairTime = TransformTimeOtlTime(otlTime);
            strm >> tempDevicRepair->FaultReason;
            strm >> tempDevicRepair->RepairResults;
            strm >> tempDevicRepair->RepairManager;
            strm >> tempDevicRepair->RepairUnit;
            strm >> tempDevicRepair->Remarks;

            ptResult.push_back(tempDevicRepair);
        }
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::AddDeviceRun(const otl_connect &dbconnect, const std::shared_ptr<DeviceRunInfo> &ptResult, uint32_t & ulRecordingID)
{
    /*if (NameExists(dbconnect, "�豸", "�豸����", ptResult->DeviceName))
    {
    return EErrCode::EC_DATABASE_ALREADYEXISTS;
    }*/
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(INSERT INTO �豸���м�¼ (�豸����,��ʼʱ��,����ʱ��,�������,��¼Ա,��ע) OUTPUT INSERTED.��¼id VALUES (
:v_firstColumn<char[12]>,
:v_secondColumn<timestamp>,
:v_thirdColumn<timestamp>,
:v_fourthColumn<char[200]>,
:v_fifthColumn<char[12]>,
:v_sixthColumn<char[200]>))";

    try
    {
        strm.set_commit(0);
        strm.open(strSQL.size(), strSQL.c_str(), db, otl_implicit_select);

        strm << ptResult->DeviceName;
        strm << TransformTimeString(ptResult->StartTime);
        strm << TransformTimeString(ptResult->EndTime);
        strm << ptResult->Operation;
        strm << ptResult->Recorder;
        strm << ptResult->Remarks;

        // û�з���ID���׳��쳣
        if (strm.eof()) throw otl_exception(OTL_CUSTOM_EXCEPTION_IDENTITY_INVALID, 0);

        // ��ȡ�����ID
        strm >> ulRecordingID;

        strm.flush();
        // �ύ����
        db.commit();
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::DeleteDeviceRun(const otl_connect &dbconnect, const int name)
{
    if (!IdExists(dbconnect, "�豸���м�¼", "��¼ID", name))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }

    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(DELETE FROM �豸���м�¼ WHERE ��¼ID = ')" + to_string(name) + R"(')";

    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        strm.flush();
        strm.close();
        // �ύ����
        db.commit();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::ModifyDeviceRun(const otl_connect &dbconnect, const std::shared_ptr<DeviceRunInfo> &ptResult)
{
    if (!IdExists(dbconnect, "�豸���м�¼", "��¼ID", ptResult->recordingID))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;
    string strSQL = R"(UPDATE �豸���м�¼
        SET
        �豸���� = :v_firstColumn<char[12]>,
        ��ʼʱ�� = :v_secondColumn<timestamp>,
        ����ʱ�� = :v_thirdColumn<timestamp>,
        ������� = :v_fourthColumn<char[200]>,
        ��¼Ա = :v_fifthColumn<char[12]>,
        ��ע = :v_sixthColumn<char[200]>
        WHERE ��¼ID = :v_ID<int>)";

    try
    {
        strm.set_commit(0);
        strm.open(strSQL.size(), strSQL.c_str(), db);

        strm << ptResult->DeviceName;
        strm << TransformTimeString(ptResult->StartTime);
        strm << TransformTimeString(ptResult->EndTime);
        strm << ptResult->Operation;
        strm << ptResult->Recorder;
        strm << ptResult->Remarks;
        strm << ptResult->recordingID;

        strm.flush();
        strm.close();
        // �ύ����
        db.commit();
    }
    catch (otl_exception &e)
    {
        strm.close();
        // �ع�����
        db.rollback();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::SearchDeviceRun(const otl_connect &dbconnect, const int name, list<shared_ptr<DeviceRunInfo>> &ptResult)
{
    if (-1 != name && !IdExists(dbconnect, "�豸���м�¼", "��¼ID", name))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(SELECT ��¼ID,�豸����,��ʼʱ��,����ʱ��,�������,��¼Ա,��ע FROM �豸���м�¼)";
    if (-1 != name)
    {
        strSQL += R"( WHERE ��¼ID = ')" + to_string(name) + R"(')";
    }
    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        // �ж��Ƿ��н��
        while (!strm.eof())
        {
            std::shared_ptr<DeviceRunInfo> tempDevicRepair = make_shared<DeviceRunInfo>();
            strm >> tempDevicRepair->recordingID;
            strm >> tempDevicRepair->DeviceName;
            otl_datetime otlTime;
            strm >> otlTime;
            tempDevicRepair->StartTime = TransformTimeOtlTime(otlTime);
            strm >> otlTime;
            tempDevicRepair->EndTime = TransformTimeOtlTime(otlTime);
            strm >> tempDevicRepair->Operation;
            strm >> tempDevicRepair->Recorder;
            strm >> tempDevicRepair->Remarks;
            ptResult.push_back(tempDevicRepair);
        }
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::SearchDeviceRun(const otl_connect &dbconnect, const string name, list<shared_ptr<DeviceRunInfo>> &ptResult)
{
    if (!NameExists(dbconnect, "�豸���м�¼", "�豸����", name))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(SELECT ��¼ID,�豸����,��ʼʱ��,����ʱ��,�������,��¼Ա,��ע  FROM �豸���м�¼)";
    if (!name.empty())
    {
        strSQL += R"( WHERE �豸���� = ')" + name + R"(')";
    }
    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        // �ж��Ƿ��н��
        while (!strm.eof())
        {
            std::shared_ptr<DeviceRunInfo> tempDevicRepair = make_shared<DeviceRunInfo>();
            strm >> tempDevicRepair->recordingID;
            strm >> tempDevicRepair->DeviceName;
            otl_datetime otlTime;
            strm >> otlTime;
            tempDevicRepair->StartTime = TransformTimeOtlTime(otlTime);
            strm >> otlTime;
            tempDevicRepair->EndTime = TransformTimeOtlTime(otlTime);
            strm >> tempDevicRepair->Operation;
            strm >> tempDevicRepair->Recorder;
            strm >> tempDevicRepair->Remarks;
            ptResult.push_back(tempDevicRepair);
        }
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::SearchSubSysVariableInfo(const otl_connect &dbconnect, shared_ptr<SubSysInfo> &ptResult)
{
    if (!NameExists(dbconnect, "BMS_SubSystem", "", ""))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(SELECT SubID,SubName FROM BMS_SubSystem)";
    
    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        // �ж��Ƿ��н��
        while (!strm.eof())
        {
            string subID = "";
            string subName = "";
            strm >> subID;
            strm >> subName;
            if (!subID.empty() && !subName.empty())
            {
                ptResult->lstSubSys.insert(map<string, string>::value_type(subID, subName));
            }
        }
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::SearchDeviceVariableInfo(const otl_connect &dbconnect, const string name, shared_ptr<DeviceVarInfo> &ptResult)
{
    /*if (!NameExists(dbconnect, "BMS_Device", "", name))
    {
        return EErrCode::EC_DATABASE_NODATA;
    }*/
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(SELECT DeviceID,DeviceName FROM BMS_Device)";
    if (!name.empty())
    {
        strSQL += R"( WHERE DeviceID like ')" + name + R"(%')";
    }
    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);
        if (strm.eof())
        {
            strm.close();
            return EErrCode::EC_DATABASE_NODATA;
        }
        // �ж��Ƿ��н��
        while (!strm.eof())
        {
            string deviceID = "";
            string deviceName = "";
            strm >> deviceID;
            strm >> deviceName;
            if (!deviceID.empty() && !deviceName.empty())
            {
                ptResult->lstDevice.insert(map<string, string>::value_type(deviceID, deviceName));
            }
        }
        ptResult->SysID = name;
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::SearchTagVariableInfo(const otl_connect &dbconnect, const string name, shared_ptr<TagVarInfo> &ptResult)
{
    /*if (!NameExists(dbconnect, "BMS_Tag", "", name))
    {
    return EErrCode::EC_DATABASE_NODATA;
    }*/
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(SELECT T.TagID, T.TagName FROM BMS_Tag T WHERE )";
    if (!name.empty())
    {
        strSQL += R"( T.TagID like ')" + name + R"(%' AND )";
    }
    strSQL += R"(T.iTagId NOT IN(SELECT ������� FROM �豸������ WHERE ������� is not null)
        AND T.iTagId NOT IN(SELECT �ɼ�������� FROM ������ WHERE �ɼ�������� is not null))";
    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);
        if (strm.eof())
        {
            strm.close();
            return EErrCode::EC_DATABASE_NODATA;
        }
        // �ж��Ƿ��н��
        while (!strm.eof())
        {
            string tagID = "";
            string tagName = "";
            strm >> tagID;
            strm >> tagName;
            if (!tagID.empty() && !tagName.empty())
            {
                ptResult->lstTag.insert(map<string, string>::value_type(tagID, tagName));
            }
        }
        ptResult->DevID = name;
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::SearchTagVarWithoutPar(const otl_connect &dbconnect, const string name, shared_ptr<TagVarInfo> &ptResult)
{
    if (name.empty())
    {
        return EErrCode::EC_DATABASE_FAIL;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strSQL = R"(SELECT TagID,TagName FROM ( SELECT * FROM BMS_Tag A WHERE A.TagID like ')" + name + R"(%') D,)";
    strSQL += R"( �豸������ B,������ C WHERE D.TagID != B.������� AND D.TagID != C.�ɼ��������)";

    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);
        if (strm.eof())
        {
            strm.close();
            return EErrCode::EC_DATABASE_NODATA;
        }
        // �ж��Ƿ��н��
        while (!strm.eof())
        {
            string tagID = "";
            string tagName = "";
            strm >> tagID;
            strm >> tagName;
            if (!tagID.empty() && !tagName.empty())
            {
                ptResult->lstTag.insert(map<string, string>::value_type(tagID, tagName));
            }
        }
        ptResult->DevID = name;
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::SearchMeterValueInfo(const otl_connect &dbconnect, const shared_ptr<ValueRequestInfo> ptRequestInfo, shared_ptr<ResponseMeterValueInfo> &ptResult)
{
    if (!ptRequestInfo)
    {
        LOG_ERROR << "SearchMeterValueInfo ptRequestInfo is null!";
        return EErrCode::EC_DATABASE_FAIL;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strTempSQL = R"(select T.��������,T.����������,B.IOGlobalId,C.TagName,B.IOValue,B.IOTime 
        from (select * from ������ A)";
    if (!ptRequestInfo->strName.empty())
    {
        strTempSQL += R"( WHERE A.�������� = ')" + ptRequestInfo->strName + R"(')";//:v_meterNum<char[12]>)";
    }
    strTempSQL += R"() as T, BMS_IOValue B, BMS_Tag C
        WHERE T.�ɼ�������� = B.IOGlobalId AND T.�ɼ�������� = C.iTagId and B.IOTime > ')";

    strTempSQL += TransformTimeUnint64(ptRequestInfo->publicInfo.ullStartTime) + R"(' and B.IOTime < ')";
    strTempSQL += TransformTimeUnint64(ptRequestInfo->publicInfo.ullEndTime) + R"(')";

    string strSQL = R"(select top )" + to_string(ptRequestInfo->publicInfo.pageSize) 
        + R"( o.* from (select row_number() over(order by oo.IOTime desc) as rownumber,* 
        from ()" 
        + strTempSQL 
        + R"() as oo) as o where o.rownumber>)"
        + to_string(ptRequestInfo->publicInfo.pageSize * (ptRequestInfo->publicInfo.pageNum - 1));

    string strSQLCount = R"(SELECT COUNT(0) FROM ()" + strTempSQL + R"() M)";
    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);
        /*if (!ptRequestInfo->strName.empty())
        {
            strm << ptRequestInfo->strName;
        }*/

        if (strm.eof())
        {
            strm.close();
            return EErrCode::EC_DATABASE_NODATA;
        }
        // �ж��Ƿ��н��
        while (!strm.eof())
        {
            MeterValueInfo meterValueInfo;
            int rowID;
            strm >> rowID;
            strm >> meterValueInfo.meterNumber;
            strm >> meterValueInfo.meterType;
            strm >> meterValueInfo.valueInfo.ulId;
            strm >> meterValueInfo.valueInfo.tagName;
            strm >> meterValueInfo.valueInfo.tagValue;
            otl_datetime otlTime;
            strm >> otlTime;
            meterValueInfo.valueInfo.collectionTime = TransformTimeOtlTime(otlTime);
            
            if (!meterValueInfo.meterNumber.empty() && 0 != meterValueInfo.valueInfo.ulId)
            {
                ptResult->lstValueInfo.push_back(meterValueInfo);
            }
        }
        
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }

    try
    {
        strm.open(strSQLCount.size(), strSQLCount.c_str(), db);
        /*if (!ptRequestInfo->strName.empty())
        {
            strm << ptRequestInfo->strName;
        }*/
        if (!strm.eof())
        {
            int nTotal = 0;
            strm >> nTotal;

            ptResult->pageTotal = nTotal / ptRequestInfo->publicInfo.pageSize;
            if (0 != (nTotal % ptRequestInfo->publicInfo.pageSize))
                ptResult->pageTotal += 1;
        }
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    ptResult->pageSize = ptRequestInfo->publicInfo.pageSize;
    ptResult->pageNum = ptRequestInfo->publicInfo.pageNum;
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::SearchDeviceValueInfo(const otl_connect &dbconnect, const shared_ptr<ValueRequestInfo>  ptRequestInfo, shared_ptr<DeviceValueInfo> &ptResult)
{
    if (!ptRequestInfo)
    {
        LOG_ERROR << "SearchDeviceValueInfo ptRequestInfo is null!";
        return EErrCode::EC_DATABASE_FAIL;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strTempSQL = R"(select B.IOGlobalId,C.TagName,B.IOValue,B.IOTime
            from (select ������� from �豸������ A)";
    if (!ptRequestInfo->strName.empty())
    {
        strTempSQL += R"(  where A.�豸���� = ')" + ptRequestInfo->strName + R"(')";
    }
    strTempSQL += R"() as T,BMS_IOValue B,BMS_Tag C
            where T.������� = B.IOGlobalId and T.������� = C.iTagId and B.IOTime > ')";

    strTempSQL += TransformTimeUnint64(ptRequestInfo->publicInfo.ullStartTime) + R"(' and B.IOTime < ')";
    strTempSQL += TransformTimeUnint64(ptRequestInfo->publicInfo.ullEndTime) + R"(')";

    string strSQL = R"(select top )" + to_string(ptRequestInfo->publicInfo.pageSize)
        + R"( o.* from (select row_number() over(order by oo.IOTime desc) as rownumber,* 
        from ()"
        + strTempSQL
        + R"() as oo) as o where o.rownumber>)"
        + to_string(ptRequestInfo->publicInfo.pageSize * (ptRequestInfo->publicInfo.pageNum - 1));

    string strSQLCount = R"(SELECT COUNT(0) FROM ()" + strTempSQL + R"() M)";
    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        if (strm.eof())
        {
            strm.close();
            return EErrCode::EC_DATABASE_NODATA;
        }
        // �ж��Ƿ��н��
        while (!strm.eof())
        {
            ValueInfo deviceValueInfo;
            int rowID;
            strm >> rowID;
            strm >> deviceValueInfo.ulId;
            strm >> deviceValueInfo.tagName;
            strm >> deviceValueInfo.tagValue;
            otl_datetime otlTime;
            strm >> otlTime;
            deviceValueInfo.collectionTime = TransformTimeOtlTime(otlTime);

            if (0 != deviceValueInfo.ulId)
            {
                ptResult->lstValueInfo.push_back(deviceValueInfo);
            }
        }
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }

    try
    {
        strm.open(strSQLCount.size(), strSQLCount.c_str(), db);
        
        if (!strm.eof())
        {
            int nTotal = 0;
            strm >> nTotal;

            ptResult->pageTotal = nTotal / ptRequestInfo->publicInfo.pageSize;
            if (0 != (nTotal % ptRequestInfo->publicInfo.pageSize))
                ptResult->pageTotal += 1;
        }
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    ptResult->pageSize = ptRequestInfo->publicInfo.pageSize;
    ptResult->pageNum = ptRequestInfo->publicInfo.pageNum;
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::SearchVariableValueInfo(const otl_connect &dbconnect, const shared_ptr<ValueRequestInfo>  ptRequestInfo, shared_ptr<DeviceValueInfo> &ptResult)
{
    if (!ptRequestInfo)
    {
        LOG_ERROR << "SearchDeviceValueInfo ptRequestInfo is null!";
        return EErrCode::EC_DATABASE_FAIL;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strTempSQL = R"(select B.IOGlobalId,C.TagName,B.IOValue,B.IOTime
            from )";
    strTempSQL += R"(BMS_IOValue B,BMS_Tag C
            where B.IOGlobalId = C.iTagId and B.IOTime > ')";

    strTempSQL += TransformTimeUnint64(ptRequestInfo->publicInfo.ullStartTime) + R"(' and B.IOTime < ')";
    strTempSQL += TransformTimeUnint64(ptRequestInfo->publicInfo.ullEndTime) + R"(')";

    string strSQL = R"(select top )" + to_string(ptRequestInfo->publicInfo.pageSize)
        + R"( o.IOGlobalId,o.TagName,o.IOValue,o.IOTime from (select row_number() over(order by oo.IOTime desc) as rownumber,* 
        from ()"
        + strTempSQL
        + R"() as oo) as o where o.rownumber>)"
        + to_string(ptRequestInfo->publicInfo.pageSize * (ptRequestInfo->publicInfo.pageNum-1));

    string strSQLCount = R"(SELECT COUNT(0) FROM ()" + strTempSQL + R"() T)";

    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);
        
        if (strm.eof())
        {
            strm.close();
            return EErrCode::EC_DATABASE_NODATA;
        }
        // �ж��Ƿ��н��
        for (; 0 == strm.eof();)
        {
            ValueInfo deviceValueInfo;
            /*int rowID = 0;
            strm >> rowID;*/
            strm >> deviceValueInfo.ulId;
            strm >> deviceValueInfo.tagName;
            strm >> deviceValueInfo.tagValue;
            otl_datetime otlTime;
            strm >> otlTime;
            deviceValueInfo.collectionTime = TransformTimeOtlTime(otlTime);

            if (0 != deviceValueInfo.ulId)
            {
                ptResult->lstValueInfo.push_back(deviceValueInfo);
            }
        }
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }

    try
    {
        strm.open(strSQLCount.size(), strSQLCount.c_str(), db);
        
        if (!strm.eof())
        {
            int nTotal = 0;
            strm >> nTotal;

            ptResult->pageTotal = nTotal / ptRequestInfo->publicInfo.pageSize;
            if (0 != (nTotal % ptRequestInfo->publicInfo.pageSize))
                ptResult->pageTotal += 1;
        }
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    ptResult->pageSize = ptRequestInfo->publicInfo.pageSize;
    ptResult->pageNum = ptRequestInfo->publicInfo.pageNum;
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::SearchHistoryEventExInfo(const otl_connect &dbconnect, const shared_ptr<HistoryEventRequestInfo>  ptRequestInfo, shared_ptr<HistoryEventInfo> &ptResult)
{
    if (!ptRequestInfo)
    {
        LOG_ERROR << "SearchDeviceValueInfo ptRequestInfo is null!";
        return EErrCode::EC_DATABASE_FAIL;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strTempSQL = R"(select EventTime,EventID,UserName,EventRecord,AccessLevel,EventType
            from BMS_Event 
            where EventTime > ')";

    strTempSQL += TransformTimeUnint64(ptRequestInfo->publicInfo.ullStartTime) + R"(' and EventTime < ')";
    strTempSQL += TransformTimeUnint64(ptRequestInfo->publicInfo.ullEndTime) + R"(')";

    string strSQL = R"(select top )" + to_string(ptRequestInfo->publicInfo.pageSize)
        + R"( o.* from (select row_number() over(order by oo.EventTime desc) as rownumber,* 
        from ()"
        + strTempSQL
        + R"() as oo) as o where o.rownumber>)"
        + to_string(ptRequestInfo->publicInfo.pageSize * (ptRequestInfo->publicInfo.pageNum - 1));

    string strSQLCount = R"(SELECT COUNT(0) FROM ()" + strTempSQL + R"() M)";
    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        if (strm.eof())
        {
            strm.close();
            return EErrCode::EC_DATABASE_NODATA;
        }
        // �ж��Ƿ��н��
        while (!strm.eof())
        {
            KEventInfo eventInfo;
            int rowID;
            strm >> rowID;
            string strTemp = "";
            strm >> strTemp;
            ptime bTime = time_from_string(strTemp);
            time_t tempTime = boost::posix_time::to_time_t(bTime);
            eventInfo.ullTimeMs = tempTime * 1000;
            strm >> strTemp;
            eventInfo.ulID = boost::lexical_cast<int>(strTemp);
            strm >> eventInfo.strUserName;
            strm >> eventInfo.strRecord;
            strm >> strTemp;
            eventInfo.ulAccessLevel = boost::lexical_cast<int>(strTemp);
            strm >> strTemp;
            eventInfo.eType = boost::lexical_cast<int>(strTemp);

            //if (0 != eventInfo.ulID)
            {
                ptResult->lstValueInfo.push_back(eventInfo);
            }
        }
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }

    try
    {
        strm.open(strSQLCount.size(), strSQLCount.c_str(), db);
        if (!strm.eof())
        {
            int nTotal = 0;
            strm >> nTotal;

            ptResult->pageTotal = nTotal / ptRequestInfo->publicInfo.pageSize;
            if (0 != (nTotal % ptRequestInfo->publicInfo.pageSize))
                ptResult->pageTotal += 1;
        }
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    ptResult->pageSize = ptRequestInfo->publicInfo.pageSize;
    ptResult->pageNum = ptRequestInfo->publicInfo.pageNum;
    return EErrCode::EC_SUCCESS;
}

EErrCode BMSInfoManager::SearchHistoryAlarmExInfo(const otl_connect &dbconnect, const shared_ptr<HistoryAlarmRequestInfo>  ptRequestInfo, shared_ptr<HistoryAlarmInfo> &ptResult)
{
    if (!ptRequestInfo)
    {
        LOG_ERROR << "SearchDeviceValueInfo ptRequestInfo is null!";
        return EErrCode::EC_DATABASE_FAIL;
    }
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;

    string strTempSQL = R"(select AlarmGlobalID,AlarmTime,AlarmID,AlarmCategory,AlarmType,AlarmPriority,AlarmSource,AlarmValue,AlarmMessage,AlarmLimit,AlarmAck
            from BMS_Alarm 
            where AlarmTime > ')";

    strTempSQL += TransformTimeUnint64(ptRequestInfo->publicInfo.ullStartTime) + R"(' and AlarmTime < ')";
    strTempSQL += TransformTimeUnint64(ptRequestInfo->publicInfo.ullEndTime) + R"(')";

    string strSQL = R"(select top )" + to_string(ptRequestInfo->publicInfo.pageSize)
        + R"( o.* from (select row_number() over(order by oo.AlarmTime desc) as rownumber,* 
        from ()"
        + strTempSQL
        + R"() as oo) as o where o.rownumber>)"
        + to_string(ptRequestInfo->publicInfo.pageSize * (ptRequestInfo->publicInfo.pageNum - 1));

    string strSQLCount = R"(SELECT COUNT(0) FROM ()" + strTempSQL + R"() M)";

    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);

        if (strm.eof())
        {
            strm.close();
            return EErrCode::EC_DATABASE_NODATA;
        }
        // �ж��Ƿ��н��
        while (!strm.eof())
        {
            KAlarmInfoEx alarmInfo;
            int rowID;
            strm >> rowID;

            string strTemp = "";
            strm >> strTemp;
            alarmInfo.ulGID = boost::lexical_cast<int>(strTemp);
            strm >> strTemp;
            ptime bTime = time_from_string(strTemp);
            time_t tempTime = boost::posix_time::to_time_t(bTime);
            alarmInfo.ullTimeMs = tempTime * 1000;
            strm >> strTemp;
            alarmInfo.ulID = boost::lexical_cast<int>(strTemp);
            strm >> strTemp;
            alarmInfo.ulCategory = boost::lexical_cast<int>(strTemp);
            strm >> strTemp;
            alarmInfo.ulType = boost::lexical_cast<int>(strTemp);
            strm >> strTemp;
            alarmInfo.ulPriority = boost::lexical_cast<int>(strTemp);
            strm >> alarmInfo.strSource;
            strm >> alarmInfo.strValue;
            strm >> alarmInfo.strMessage;
            strm >> alarmInfo.strLimit;
            strm >> strTemp;
            alarmInfo.bAck = boost::lexical_cast<bool>(strTemp);
            if (0 != alarmInfo.ulGID)
            {
                ptResult->lstValueInfo.push_back(alarmInfo);
            }
        }
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }

    try
    {
        strm.open(strSQLCount.size(), strSQLCount.c_str(), db);

        if (!strm.eof())
        {
            int nTotal = 0;
            strm >> nTotal;

            ptResult->pageTotal = nTotal / ptRequestInfo->publicInfo.pageSize;
            if (0 != (nTotal % ptRequestInfo->publicInfo.pageSize))
                ptResult->pageTotal += 1;
        }
        strm.close();
    }
    catch (otl_exception &e)
    {
        strm.close();
        cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return EErrCode::EC_DATABASE_FAIL;
    }
    ptResult->pageSize = ptRequestInfo->publicInfo.pageSize;
    ptResult->pageNum = ptRequestInfo->publicInfo.pageNum;
    return EErrCode::EC_SUCCESS;
}

bool BMSInfoManager::NameExists(const otl_connect &dbconnect, const string tableName, const string keyName, const string &keyValue)
{
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;
    string strSQL = R"(SELECT COUNT(0) FROM )";
    strSQL += tableName;
    if (!keyValue.empty())
    {
        strSQL += R"( WHERE )" + keyName + R"( = ')" + keyValue + R"(')";
    }
    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);
        int count = 0;
        strm >> count;
        strm.close();
        if (count > 0)
        {
            return true;
        }
    }
    catch (otl_exception &e)
    {
        strm.close();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return false;
    }
    return false;
}

bool BMSInfoManager::IdExists(const otl_connect &dbconnect, const string tableName, const string keyName, const int &IdValue)
{
    otl_connect &db = const_cast<otl_connect &>(dbconnect);
    otl_nocommit_stream strm;
    string strSQL = R"(SELECT COUNT(0) FROM )";
    strSQL += tableName;
    if (0 != IdValue)
    {
        strSQL += R"( WHERE )" + keyName + R"( = ')" + to_string(IdValue) + R"(')";
    }
    try
    {
        strm.open(strSQL.size(), strSQL.c_str(), db);
        int count = 0;
        strm >> count;
        strm.close();
        if (count > 0)
        {
            return true;
        }
    }
    catch (otl_exception &e)
    {
        strm.close();
        LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
        return false;
    }
    return false;
}

otl_datetime BMSInfoManager::TransformTimeString(const string &strTime)
{
    try {
        ptime bTime = time_from_string(strTime);
        tm ptmp = to_tm(bTime);
        otl_datetime otlTime(ptmp.tm_year + 1900,
            ptmp.tm_mon + 1,
            ptmp.tm_mday,
            ptmp.tm_hour,
            ptmp.tm_min,
            ptmp.tm_sec,
            0,
            OTL_CUSTOM_TIME_PRECISION);
        return otlTime;
    }
    catch (const std::exception & e) {
        LOG_ERROR << e.what();
        return m_otlTime;
    }
}

string BMSInfoManager::TransformTimeOtlTime(const otl_datetime &otlTime)
{
    string strTime = "";
    if (otlTime.year != 1900)
    {
        strTime = to_string(otlTime.year)
            + "-" + to_string(otlTime.month)
            + "-" + to_string(otlTime.day)
            + " " + to_string(otlTime.hour)
            + ":" + to_string(otlTime.minute)
            + ":" + to_string(otlTime.second);
    }
    return strTime;
}

string BMSInfoManager::TransformTimeUnint64(uint64_t ulltime)
{
    uint64_t  ullM = ulltime / 1000;
    struct tm *ptmTmp = localtime((const time_t*)&ullM);

    char szTime[20];
    int year = ptmTmp->tm_year + 1900;
    int mon = ptmTmp->tm_mon + 1;
    int day = ptmTmp->tm_mday;
    int hour = ptmTmp->tm_hour;
    int min = ptmTmp->tm_min;
    int sec = ptmTmp->tm_sec;
    sprintf_s(szTime, "%4d-%02d-%02d %02d:%02d:%02d",
        year,
        mon,
        day,
        hour,
        min,
        sec);
    string sTime = szTime;
    return sTime;
}


EErrCode BMSInfoManager::SearchOverViewInfo(const otl_connect &dbconnect, std::shared_ptr<OverViewInfo> &ptResult)
{
	shared_ptr<SubSysInfo>  subsystem = make_shared<SubSysInfo>();
	SearchSubSysVariableInfo(dbconnect, subsystem);
	map<string, string>::iterator iter;
	
	for (iter = subsystem->lstSubSys.begin(); iter != subsystem->lstSubSys.end(); iter++)
	{
		std::shared_ptr<DeviceVarInfo>  device = make_shared<DeviceVarInfo>();
		SearchDeviceVariableInfo(dbconnect, iter->first, device);
		map<string, string>::iterator iterdevice;
		shared_ptr<CommonData> common = make_shared<CommonData>();
		int count = 0;
		for (iterdevice = device->lstDevice.begin(); iterdevice != device->lstDevice.end(); iterdevice++)
		{
			std::shared_ptr<TagVarInfo>  tagInfo = make_shared<TagVarInfo>();
			SearchTagVariableInfo(dbconnect, iterdevice->first, tagInfo);
			count += tagInfo->lstTag.size();
		}
		common->subsystemId = iter->first;
		common->deviceCount = device->lstDevice.size();
		common->tagCount = count;
		ptResult->overData.push_back(common);
	}

	return EErrCode::EC_SUCCESS;
}


EErrCode BMSInfoManager::SearchSystemAlarmCountInfo(const otl_connect &dbconnect, shared_ptr<Subsystem_Alarm> &ptResult)
{
	shared_ptr<SubSysInfo>  subsystem = make_shared<SubSysInfo>();
	SearchSubSysVariableInfo(dbconnect, subsystem);
	map<string, string>::iterator iter;

	for (iter = subsystem->lstSubSys.begin(); iter != subsystem->lstSubSys.end(); iter++)
	{
		int id = atoi(iter->first.c_str());
		if (0 == id)
		{
			LOG_ERROR << "SearchDeviceValueInfo ptRequestInfo is null!";
			return EErrCode::EC_DATABASE_FAIL;
		}
		int minId = (id << 24);
		int maxId = ((id + 1) << 24);
		otl_connect &db = const_cast<otl_connect &>(dbconnect);
		otl_nocommit_stream strm;

		string strTempSQL = R"(select AlarmGlobalID,AlarmTime,AlarmID,AlarmCategory,AlarmType,AlarmPriority,AlarmSource,AlarmValue,AlarmMessage,AlarmLimit,AlarmAck
             from BMS_Alarm 
            where AlarmGlobalID > ')";

		strTempSQL += to_string(minId) + R"(' and AlarmGlobalID < ')";
		strTempSQL += to_string(maxId) + R"(' and AlarmAck = 0)";
		//select conut(*) 
		string strSQLCount = R"(SELECT COUNT(0) FROM ()" + strTempSQL + R"() M)";
		try
		{
			strm.open(strSQLCount.size(), strSQLCount.c_str(), db);

			if (!strm.eof())
			{
				int nTotal = 0;
				strm >> nTotal;

				ptResult->alarmInfo.insert(make_pair(iter->first, nTotal));
			}
			strm.close();
		}
		catch (otl_exception &e)
		{
			strm.close();
			cout << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info << endl;
			LOG_ERROR << e.code << "��" << e.msg << " " << e.stm_text << " " << e.var_info;
			return EErrCode::EC_DATABASE_FAIL;
		}
	}

	return EErrCode::EC_SUCCESS;
}