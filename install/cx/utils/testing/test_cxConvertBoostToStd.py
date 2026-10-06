'''
Tests for cx.utils.cxConvertBoostToStd.

Run with:
    cd install && python3 -m unittest discover -s cx -v
'''
import os
import shutil
import sys
import tempfile
import unittest

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..', '..')))

from cx.utils import cxConvertBoostToStd


class TestConvertText(unittest.TestCase):

    def convert(self, text):
        return cxConvertBoostToStd.convertText(text)

    def test_pointer_typedef_and_casts(self):
        text = ('typedef boost::shared_ptr<class Image> ImagePtr;\n'
                'boost::weak_ptr<Image> w;\n'
                'ImagePtr i = boost::dynamic_pointer_cast<Image>(data);\n'
                'ImagePtr j = boost::static_pointer_cast<Image>(data);\n'
                'ImagePtr k = boost::make_shared<Image>();\n')
        expected = ('#include <memory>\n'
                    'typedef std::shared_ptr<class Image> ImagePtr;\n'
                    'std::weak_ptr<Image> w;\n'
                    'ImagePtr i = std::dynamic_pointer_cast<Image>(data);\n'
                    'ImagePtr j = std::static_pointer_cast<Image>(data);\n'
                    'ImagePtr k = std::make_shared<Image>();\n')
        self.assertEqual(self.convert(text), expected)

    def test_scoped_ptr_becomes_unique_ptr(self):
        self.assertEqual(self.convert('boost::scoped_ptr<A> mA;'), '#include <memory>\nstd::unique_ptr<A> mA;')

    def test_function_array_integers_unordered_map(self):
        text = 'boost::function<void()> f; boost::array<int,3> a; boost::uint64_t t; boost::int32_t s; boost::unordered_map<int,int> m;'
        expected = ('#include <functional>\n#include <array>\n#include <unordered_map>\n#include <cstdint>\n'
                    'std::function<void()> f; std::array<int,3> a; std::uint64_t t; std::int32_t s; std::unordered_map<int,int> m;')
        self.assertEqual(self.convert(text), expected)

    def test_other_boost_left_unchanged(self):
        text = ('#include <boost/bind/bind.hpp>\n'
                'boost::function_traits<F>::arity; boost::bind(&A::f, this); boost::math::round(x); boost::shared_ptrX y;\n')
        self.assertEqual(self.convert(text), text)

    def test_includes_replaced_without_duplicates(self):
        text = ('#include <boost/shared_ptr.hpp>\n'
                '#include "boost/weak_ptr.hpp"\n'
                '#include <boost/function.hpp>\n'
                '#include <vector>\n')
        expected = ('#include <memory>\n'
                    '#include <functional>\n'
                    '#include <vector>\n')
        self.assertEqual(self.convert(text), expected)

    def test_boost_include_dropped_when_std_header_already_included(self):
        text = '#include <memory>\n#include <boost/shared_ptr.hpp>\nint a;\n'
        self.assertEqual(self.convert(text), '#include <memory>\nint a;\n')

    def test_commented_include_left_unchanged(self):
        text = '//#include <boost/scoped_ptr.hpp>\n'
        self.assertEqual(self.convert(text), text)

    def test_crlf_line_endings_kept(self):
        text = '#include <boost/shared_ptr.hpp>\r\nboost::shared_ptr<A> a;\r\n'
        self.assertEqual(self.convert(text), '#include <memory>\r\nstd::shared_ptr<A> a;\r\n')

    def test_missing_std_header_added_after_first_include(self):
        text = '#include "cxA.h"\n#include "cxB.h"\ntypedef boost::shared_ptr<class A> APtr;\n'
        expected = '#include "cxA.h"\n#include <memory>\n#include "cxB.h"\ntypedef std::shared_ptr<class A> APtr;\n'
        self.assertEqual(self.convert(text), expected)

    def test_missing_std_header_added_after_include_guard(self):
        text = '#ifndef CXA_H\n#define CXA_H\ntypedef boost::shared_ptr<class A> APtr;\n#endif\n'
        expected = '#ifndef CXA_H\n#define CXA_H\n#include <memory>\ntypedef std::shared_ptr<class A> APtr;\n#endif\n'
        self.assertEqual(self.convert(text), expected)

    def test_file_without_boost_left_unchanged(self):
        text = '#include "cxA.h"\nstd::shared_ptr<A> a;\n'
        self.assertEqual(self.convert(text), text)

    def test_add_missing_includes_on_already_converted_text(self):
        text = '#include "cxA.h"\nstd::shared_ptr<A> a; std::function<void()> f;\n'
        expected = '#include "cxA.h"\n#include <memory>\n#include <functional>\nstd::shared_ptr<A> a; std::function<void()> f;\n'
        self.assertEqual(cxConvertBoostToStd.addMissingIncludes(text), expected)

    def test_conversion_is_idempotent(self):
        text = '#include <boost/shared_ptr.hpp>\nboost::shared_ptr<A> a;\n'
        once = self.convert(text)
        self.assertEqual(self.convert(once), once)


class TestConvertFiles(unittest.TestCase):

    def setUp(self):
        self.dir = tempfile.mkdtemp()

    def tearDown(self):
        shutil.rmtree(self.dir)

    def write(self, relPath, text):
        path = os.path.join(self.dir, relPath)
        os.makedirs(os.path.dirname(path), exist_ok=True)
        with open(path, 'w') as f:
            f.write(text)
        return path

    def read(self, path):
        with open(path) as f:
            return f.read()

    def test_converts_cpp_files_only_and_skips_git_folders(self):
        header = self.write('src/a.h', 'boost::shared_ptr<A> a;\n')
        script = self.write('src/b.py', 'boost::shared_ptr\n')
        gitFile = self.write('.git/c.h', 'boost::shared_ptr<A> a;\n')
        cxConvertBoostToStd.main([self.dir])
        self.assertEqual(self.read(header), '#include <memory>\nstd::shared_ptr<A> a;\n')
        self.assertEqual(self.read(script), 'boost::shared_ptr\n')
        self.assertEqual(self.read(gitFile), 'boost::shared_ptr<A> a;\n')


if __name__ == '__main__':
    unittest.main()
