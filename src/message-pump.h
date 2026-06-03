// message-pump.h

class IMessagePump {
public:
    virtual int WaitAndPumpMessages(HANDLE process) = 0;
};

class MessagePump : public IMessagePump {
public:
    MessagePump(IWindowsApi *api);

    int WaitAndPumpMessages(HANDLE process) override;

private:
    IWindowsApi *m_api;
};
