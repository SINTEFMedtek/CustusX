#####################################################
# 
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.12.17
#
#####################################################
#import paramiko
import os
import os.path
import cx.cxUtilities
import platform
import getpass
import pkg_resources


class RemoteFileTransfer:
    def __init__(self):
        self.paramiko = cx.cxUtilities.try_paramiko_import()
        print 'paramiko version:', pkg_resources.get_distribution("paramiko").version

    def connect(self, remoteServer):
        self.host_name = remoteServer

        self.transport = self.paramiko.Transport((remoteServer, 22))
        self.transport.start_client()
        if not self._authenticate():
            return False
        
        self.sftp = self.transport.open_sftp_client()
        return True
        
    def close(self):
        self.sftp.close()
        self.transport.close()

    def _getUsername(self):
        '''
        Read username from ssh config
        https://github.com/paramiko/paramiko/pull/97
        '''
        conf = self.paramiko.SSHConfig()
        path_to_config = os.path.expanduser('~/.ssh/config')
        if os.path.exists(path_to_config):
            conf.parse(open(path_to_config))
        host = conf.lookup(self.host_name)
        if 'user' in host:
            return host['user']
        else:
            return getpass.getuser()
    
    def _authenticate(self):
        agent = self.paramiko.Agent()
        rsa_private_key = self._getPrivateKey()
        username = self._getUsername()
        print 'Username ', username
        agent_keys = agent.get_keys() + (rsa_private_key,)
        success = False
        if len(agent_keys) == 0:
            print 'failed to authenticate, no keys'
            return False
        for key in agent_keys:
            success = self._authenticate_using_key(username, key)
            if success:
                break
        if success:
            print "Authenticated successfully"
        return success

    def _authenticate_using_key(self, username, key):
        print 'Trying ssh-agent key %s' % key.get_fingerprint().encode('hex'),
        try:
            self.transport.auth_publickey(username, key)
            print '... success!'
            print "more"
        except self.paramiko.SSHException, e:
            print '... failed!', e
            return False
        return True

    def _getPrivateKey(self):
        rsa_private_key = os.path.expanduser(os.path.join("~", ".ssh", "id_rsa"))
        try:
            ki = self.paramiko.RSAKey.from_private_key_file(rsa_private_key)
        except Exception, e:
            print 'Failed loading %s, e=%s' % (rsa_private_key, e)
        return ki

    def copyFolderContentsToRemoteServer(self, sourcePath, destPath):
        self.remote_mkdir(destPath)
        for (root, dirs, files) in os.walk(sourcePath):
            for file in files:
                self.remote_put(os.path.join(sourcePath, file), 
                               os.path.join(destPath, file))
            for dir in dirs:
                self.copyFolderContentsToRemoteServer(os.path.join(sourcePath, dir), 
                                                      os.path.join(destPath, dir))
            break
                
    def remote_put(self, source, dest):
        dest = dest.replace('\\', '/')
        print 'remote_put %s -> %s' % (source, dest)
        self.sftp.put(source, dest)
        if platform.system() == 'Windows':
            self.sftp.chmod(dest, 0777) # default is rw for user only on win
    
    def remote_mkdir(self, path):
        path = path.replace('\\', '/')
        print 'remote_mkdir %s' % path
        try:
            self.sftp.mkdir(path)
        except IOError:
            print "%s already exists...(failed to create)"  % path
                
def copyFolderContentsToRemoteServer(remoteServer, sourcePath, destPath):
    '''
    '''
    transfer = RemoteFileTransfer()
    transfer.connect(remoteServer)
    transfer.copyFolderContentsToRemoteServer(sourcePath, destPath);
    transfer.close()

