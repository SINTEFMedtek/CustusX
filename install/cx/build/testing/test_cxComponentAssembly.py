'''
Regression tests for cx.build.cxComponentAssembly.LibraryAssembly.removeComponent().

All filesystem operations happen inside a per-test tempfile.mkdtemp()
sandbox, never against a real checkout.

Run with:
    cd install && python3 -m unittest discover -s cx/build/testing -v
'''
import os
import shutil
import sys
import tempfile
import unittest

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..')))

import cx.build.cxComponents as cxComponents
from cx.build import cxComponentAssembly


class _FakeControlData:
    '''
    A real cxInstallData.Common's getCustusXRepositoryLocation() derives the
    CX repo location from this module's own __file__ on disk -- it ignores
    any instance state entirely, so it cannot be sandboxed just by
    overriding getWorkingPath(). This is exactly what went wrong during a
    v26.09 removeComponent() test earlier: a controlData double that only
    overrode getWorkingPath() still resolved a plugin-style component's
    path() through the real getCustusXRepositoryLocation(), pointing at the
    actual CustusX checkout instead of the test sandbox. Override both here.
    '''
    def __init__(self, root):
        self._root = root

    def getWorkingPath(self):
        return self._root

    def getCustusXRepositoryLocation(self):
        return [self._root, 'CX', 'CX']

    def getRepoFolderName(self):
        return self.getCustusXRepositoryLocation()[2]

    def getBuildFolder(self, component_name):
        return 'build_Release'


class _FakeComponent:
    '''Minimal stand-in implementing only what removeComponent() touches.'''
    def __init__(self, path=None, source_path=None, plugin_path=None, name='FakeComponent'):
        self._path = path
        self._source_path = source_path
        self._plugin_path = plugin_path
        self._name = name

    def setControlData(self, data):
        self.controlData = data

    def name(self):
        return self._name

    def path(self):
        return self._path

    def sourcePath(self):
        return self._source_path

    def pluginPath(self):
        return self._plugin_path


def _remove(control_data, component):
    'Call removeComponent() without going through LibraryAssembly.__init__ (which addComponent()s a whole default set of libraries).'
    assembly = cxComponentAssembly.LibraryAssembly.__new__(cxComponentAssembly.LibraryAssembly)
    assembly.controlData = control_data
    assembly.removeComponent(component)


class RemoveComponentTest(unittest.TestCase):
    def setUp(self):
        self.tmp = os.path.realpath(tempfile.mkdtemp(prefix='cxComponentAssembly_test_'))
        self.control_data = _FakeControlData(self.tmp)

    def tearDown(self):
        shutil.rmtree(self.tmp, ignore_errors=True)

    def test_regular_component_deletes_its_own_path(self):
        target = os.path.join(self.tmp, 'SomeLib')
        os.makedirs(os.path.join(target, 'src'))

        _remove(self.control_data, _FakeComponent(path=target, plugin_path=None))

        self.assertFalse(os.path.exists(target))

    def test_plugin_component_deletes_only_its_own_source_path(self):
        shared_plugins_dir = os.path.join(self.tmp, 'source', 'plugins')
        keep = os.path.join(shared_plugins_dir, 'org.custusx.keep')
        remove_me = os.path.join(shared_plugins_dir, 'org.custusx.stale')
        os.makedirs(keep)
        os.makedirs(remove_me)

        _remove(self.control_data, _FakeComponent(plugin_path=shared_plugins_dir, source_path=remove_me))

        self.assertFalse(os.path.exists(remove_me))
        self.assertTrue(os.path.exists(keep))  # the shared plugins/ folder and its siblings must survive

    def _makeFiles(self, folder, names):
        os.makedirs(folder, exist_ok=True)
        paths = [os.path.join(folder, name) for name in names]
        for path in paths:
            open(path, 'w').close()
        return paths

    def test_plugin_component_deletes_its_stale_plugin_binaries(self):
        plugins_bin = os.path.join(self.tmp, 'CX', 'build_Release', 'bin', 'plugins')
        stale = self._makeFiles(plugins_bin, ['liborg_custusx_stale.so', 'liborg_custusx_stale.dylib'])
        keep = self._makeFiles(plugins_bin, ['liborg_custusx_keep.so', 'liborg_custusx_stale_other.so'])
        shared_plugins_dir = os.path.join(self.tmp, 'CX', 'CX', 'source', 'plugins')

        _remove(self.control_data, _FakeComponent(plugin_path=shared_plugins_dir,
                                                  source_path=os.path.join(shared_plugins_dir, 'org.custusx.stale')))

        for path in stale:
            self.assertFalse(os.path.exists(path), path)
        for path in keep:
            self.assertTrue(os.path.exists(path), path)

    def test_plugin_component_deletes_its_stale_windows_plugin_binaries(self):
        bin_path = os.path.join(self.tmp, 'CX', 'build_Release', 'bin')
        stale = self._makeFiles(bin_path, ['org_custusx_stale.dll', 'org_custusx_stale.pdb'])
        keep = self._makeFiles(bin_path, ['org_custusx_keep.dll'])
        shared_plugins_dir = os.path.join(self.tmp, 'CX', 'CX', 'source', 'plugins')

        _remove(self.control_data, _FakeComponent(plugin_path=shared_plugins_dir,
                                                  source_path=os.path.join(shared_plugins_dir, 'org.custusx.stale')))

        for path in stale:
            self.assertFalse(os.path.exists(path), path)
        for path in keep:
            self.assertTrue(os.path.exists(path), path)

    def test_missing_target_is_a_noop(self):
        component = _FakeComponent(path=os.path.join(self.tmp, 'DoesNotExist'), plugin_path=None)
        _remove(self.control_data, component)  # must not raise

    def test_real_plugin_component_path_resolution_stays_inside_sandbox(self):
        '''
        Regression guard for the actual incident: with both
        getWorkingPath() and getCustusXRepositoryLocation() overridden,
        every path a real plugin-style component can resolve through
        (path(), sourcePath(), pluginPath()) must stay inside the sandbox,
        regardless of which of controlData's methods it happens to route
        through internally.
        '''
        plugin = cxComponents.org_custusx_bronchoscopynavigation()
        plugin.setControlData(self.control_data)

        for resolved in (plugin.path(), plugin.sourcePath(), plugin.pluginPath()):
            resolved = os.path.realpath(resolved)
            self.assertEqual(
                os.path.commonpath([resolved, self.tmp]), self.tmp,
                'component path escaped the test sandbox: %s (sandbox: %s)' % (resolved, self.tmp))


if __name__ == '__main__':
    unittest.main()
