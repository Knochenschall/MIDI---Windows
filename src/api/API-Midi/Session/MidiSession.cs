﻿using MidiService.Protocol.Messages.Base;
using MidiService.Protocol.Messages.Session;
using MidiService.Protocol.Serialization;
using MidiService.Protocol;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO.Pipes;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Microsoft.Windows.Midi.Session
{

    public sealed class MidiSession : IDisposable
    {
        private MidiSessionSerializableData _data;
        private bool disposedValue;

        public Guid Id => _data.Id; 
        public string Name { get => _data.Name; set => _data.Name = value; }
        
        public DateTime CreatedTime { get => _data.CreatedTime; internal set => _data.CreatedTime = value; }

        // TODO: Log level

        // ================================================================================
        #region Session lifetime

        public static MidiSession Create(string name, MidiSessionCreateOptions options)
        {
            MidiSessionSerializableData data = new MidiSessionSerializableData();

            data.ClientId = Guid.NewGuid();
            data.ProcessName = Process.GetCurrentProcess().ProcessName;
            data.ProcessId = Process.GetCurrentProcess().Id;

            // Send connection request

            var request = new CreateSessionRequestMessage()
            {
                Header = new RequestMessageHeader()
                {
                    ClientId = data.ClientId,
                    ClientRequestId = Guid.NewGuid(),
                //TODO    ClientVersion = _clientVersion.ToString(),
                },

                Name = name,
                ProcessName = data.ProcessName,
                ProcessId = data.ProcessId 
            };

            using (NamedPipeClientStream sessionPipe = new NamedPipeClientStream(
                ".", // local machine
                MidiServiceConstants.InitialConnectionPipeName,
                PipeDirection.InOut, PipeOptions.None
                ))
            {
                // get the serializer ready
                MidiStreamSerializer serializer = new MidiStreamSerializer(sessionPipe);

                // connect to pipe. This could take a while if pipe is busy
                sessionPipe.Connect();

                // send request message
                serializer.Serialize(request);

                // wait for response (reading blocks until data arrives)
                CreateSessionResponseMessage response = serializer.Deserialize<CreateSessionResponseMessage>();

                // TODO: Check ClientRequestId values match

                if (response.Header.ClientRequestId == request.Header.ClientRequestId)
                {
                    data.CreatedTime = response.CreatedTime;
                    data.Id = response.NewSessionId;

                    MidiSession session = new MidiSession()
                    {
                        _data = data,
                    };



                    // TODO: Open the session-specific pipe 

                    if (!options.DeferDeviceEnumeration)
                    {
                        // TODO: enumerate devices and endpoints
                    }


                    return session;

                }
                else
                {
                    // Should throw an exception. We're somehow replying to someone else's message
                }

            }

            return null;

        }

        public static MidiSession Create(string name)
        {
            return Create(name, new MidiSessionCreateOptions());
        }

        public void Close()
        {
            // TODO: Tell server we're done and to remove the session. There's no response for this message
        }

        // ================================================================================
        #region IDisposable
        private void Dispose(bool disposing)
        {
            if (!disposedValue)
            {
                if (disposing)
                {
                    // TODO: dispose managed state (managed objects)
                }

                Close();

                // TODO: set large fields to null
                disposedValue = true;
            }
        }

        // TODO: override finalizer only if 'Dispose(bool disposing)' has code to free unmanaged resources
        ~MidiSession()
        {
            // Do not change this code. Put cleanup code in 'Dispose(bool disposing)' method
            Dispose(disposing: false);
        }

        public void Dispose()
        {
            // Do not change this code. Put cleanup code in 'Dispose(bool disposing)' method
            Dispose(disposing: true);
            GC.SuppressFinalize(this);
        }

        #endregion







        #endregion

        // ================================================================================
        #region Device Enumeration





        #endregion

        // ================================================================================
        // ================================================================================
        // ================================================================================





        // TODO: Consider making a partial class to break out API surface area from
        // internal comms



        // todo: Method to save local property changes to server
        // Do this instead of per-property to make the change
        // use as few messages as possible

        // TODO: Device and endpoint lifetime / CRUD


        // TODO: Enumeration of devices and endpoints


        // TODO: Utility functions


    }
}
