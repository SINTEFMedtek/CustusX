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

def copyFolderContentsToRemoteServer_non_portable(remoteServer, sourcePath, destPath):
    '''
    '''
    cmd1 = 'ssh %s "mkdir -p %s"' % (remoteServer, target)
    cmd2 = 'scp -r %s/* %s:%s' % (path, remoteServer, target)
    shell.run(cmd1)
    shell.run(cmd2)
#    cxSSH.copyFolderContentsToRemoteServer(remoteServer, path, target);

class RemoteFileTransfer:
    def __init__(self, remoteServer):
#        self.test = cx.cxUtilities.try_module_import('blabla')
        self.paramiko = cx.cxUtilities.try_paramiko_import()
        self.ssh = self.paramiko.SSHClient() 
        self.ssh.load_host_keys(os.path.expanduser(os.path.join("~", ".ssh", "known_hosts")))
        #ssh.connect(server, username=username, password=password)
        self.ssh.connect(remoteServer)
        self.sftp = self.ssh.open_sftp()
        
    def close(self):
        self.sftp.close()
        self.ssh.close()

    def copyFolderContentsToRemoteServer(self, sourcePath, destPath):
        #files = self._getFiles
#        print '=== copyFolderContentsToRemoteServer src=%s, dst=%s' % (sourcePath, destPath)
        self.remote_mkdir(destPath)
        for (root, dirs, files) in os.walk(sourcePath):
#            print "*"*20
#            print "  root", root
#            print "  dirs", dirs
#            print "  files", files
            for file in files:
                self.remote_put(os.path.join(sourcePath, file), 
                               os.path.join(destPath, file))
            for dir in dirs:
                self.copyFolderContentsToRemoteServer(os.path.join(sourcePath, dir), 
                                                      os.path.join(destPath, dir))
            break
                
    def remote_put(self, source, dest):
        print 'remote_put %s -> %s' % (source, dest)
        self.sftp.put(source, dest)
    
    
    def remote_mkdir(self, path):
        print 'remote_mkdir %s' % path
        try:
            self.sftp.mkdir(path)
        except IOError:
            print "%s already exists...(failed to create)"  % path
                
def copyFolderContentsToRemoteServer(remoteServer, sourcePath, destPath):
    '''
    '''
    transfer = RemoteFileTransfer(remoteServer)
    transfer.copyFolderContentsToRemoteServer(sourcePath, destPath);
    transfer.close()

