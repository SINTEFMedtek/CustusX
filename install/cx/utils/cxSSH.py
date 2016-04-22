#####################################################
# 
# Author: Christian Askeland, SINTEF Medical Technology
# Date:   2013.12.17
#
#####################################################
#import paramiko
import os
import os.path
import cxUtilities
import platform
import getpass
import pkg_resources

class RemoteServerID:
    '''
    Information about a remote server
    '''
    def __init__(self, server=None, path=None, user=None):
        self.server = server
        self.path = path
        self.user = user
    def get_scp_target_string(self):
        '''
        return the target part of a scp call:
          user@server:path
        '''
        retval = self.server
        if self.user!=None:
            retval = "%s@%s" % (self.user, retval)
        if self.path!=None:
            retval = "%s:%s" % (retval, self.path)
        return retval


class RemoteFileTransfer:
    def __init__(self):
        self.paramiko = cxUtilities.try_paramiko_import()
        print 'paramiko version:', pkg_resources.get_distribution("paramiko").version

    def connect(self, remoteServer, user=None):
        self.host_name = remoteServer

        print "connecting to %s, user=%s" % (remoteServer, user)
        self.client = self.paramiko.client.SSHClient()
        self.client.set_missing_host_key_policy(self.paramiko.client.AutoAddPolicy())
        self.client.load_system_host_keys()
        self.client.connect(remoteServer, 22, user)
        self.sftp = self.client.open_sftp()
        return True
        
#        self.transport = self.paramiko.Transport((remoteServer, 22))
#        self.transport.start_client()
#        if not self._authenticate(user):
#            if not self._authenticate(self._getUsername()):
#                return False
#        
#        self.sftp = self.transport.open_sftp_client()
#        return True
        
    def close(self):
        #self.sftp.close()
        self.client.close()
        #self.transport.close()

#    def _getUsername(self):
#        '''
#        Read username from ssh config
#        https://github.com/paramiko/paramiko/pull/97
#        '''
#        conf = self.paramiko.SSHConfig()
#        path_to_config = os.path.expanduser('~/.ssh/config')
#        if os.path.exists(path_to_config):
#            conf.parse(open(path_to_config))
#        host = conf.lookup(self.host_name)
#        if 'user' in host:
#            return host['user']
#        else:
#            return getpass.getuser()

#    def _authenticate(self, user):
#        if not user:
#            return False
#        agent = self.paramiko.Agent()
#        rsa_private_key = self._getPrivateKey()
#        agent_keys = agent.get_keys() + (rsa_private_key,)
#        success = False
#        if len(agent_keys) == 0:
#            print 'failed to authenticate with user %s, no keys' % user
#            return False
#        for key in agent_keys:
#            success = self._authenticate_using_key(user, key)
#            if success:
#                break
#        if success:
#            print "Authenticated successfully with user %s" % user
#        return success
    
#    def _authenticate(self):
#        username = self._getUsername()
#        success = self._authenticate(username)

#    def _authenticate_using_key(self, username, key):
#        print 'Trying ssh-agent key %s' % key.get_fingerprint().encode('hex'),
#        try:
#            self.transport.auth_publickey(username, key)
#            print '... success!'
#            print "more"
#        except self.paramiko.SSHException, e:
#            print '... failed!', e
#            return False
#        return True
#
#    def _getPrivateKey(self):
#        rsa_private_key = os.path.expanduser(os.path.join("~", ".ssh", "id_rsa"))
#        try:
#            ki = self.paramiko.RSAKey.from_private_key_file(rsa_private_key)
#        except Exception, e:
#            print 'Failed loading %s, e=%s' % (rsa_private_key, e)
#        return ki

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
            print "Failed to create %s (already exists?)"  % path
            
    def remote_rmdir(self, path):
        path = path.replace('\\', '/')
        self.remote_shell_cmd('rm -rf %s' % path)
#        print 'remote_rmdir %s' % path
#        try:
#            session = self.client.get_transport().open_channel("session")
#            session.exec_command('rm -rf %s' % path)
#            session.recv_exit_status()
#        except IOError:
#            print "Failed to remove %s"  % path

    def remote_shell_cmd(self, cmd):
        print 'Executing remote command [%s]' % cmd
        try:
            session = self.client.get_transport().open_channel("session")
            session.exec_command(cmd)
            session.recv_exit_status()
        except IOError:
            print "Failed to execute command [%s]"  % cmd
                
def copyFolderContentsToRemoteServer(remoteServer, sourcePath, destPath):
    '''
    '''
    transfer = RemoteFileTransfer()
    transfer.connect(remoteServer)
    transfer.copyFolderContentsToRemoteServer(sourcePath, destPath);
    transfer.close()

