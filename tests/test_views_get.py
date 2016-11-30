import unittest
import shutil

import requests

import os


TEST_WEBSPACE = 'tests/test_webspace'
HOST = 'http://localhost:8000'


class BasePermissionsTest(unittest.TestCase):

    def setUp(self):

        # Remove test webspace if exists
        self.tearDown()

        # Create test webspace
        os.mkdir(TEST_WEBSPACE)
        os.mkdir(TEST_WEBSPACE+'/dir1')
        os.mkdir(TEST_WEBSPACE+'/dir2')

        # Create HTML files
        open(TEST_WEBSPACE+'/index.html', 'w').write(
            "<h1>index.html</h1>")
        open(TEST_WEBSPACE+'/dir1/welcome.html', 'w').write(
            "<h1>dir1/welcome.html</h1>")
        open(TEST_WEBSPACE+'/dir1/page1.html', 'w').write(
            "<h1>dir1/page1.html</h1>")
        open(TEST_WEBSPACE+'/dir1/page2.html', 'w').write(
            "<h1>dir1/page2.html</h1>")
        open(TEST_WEBSPACE+'/dir2/index.html', 'w').write(
            "<h1>dir2/index.html</h1>")

        # Set permissions
        os.chmod(TEST_WEBSPACE+'/index.html', 777)
        os.chmod(TEST_WEBSPACE+'/dir1/welcome.html', 777)
        os.chmod(TEST_WEBSPACE+'/dir1/page1.html', 777)
        os.chmod(TEST_WEBSPACE+'/dir1/page2.html', 000)
        os.chmod(TEST_WEBSPACE+'/dir2/index.html', 777)

        os.chmod(TEST_WEBSPACE, 0777)
        os.chmod(TEST_WEBSPACE+'/dir1', 0777)
        os.chmod(TEST_WEBSPACE+'/dir2', 0000)

    def tearDown(self):
        # remove test webspace if exists
        try:
            os.chmod(TEST_WEBSPACE+'/dir1/page2.html', 777)
            os.chmod(TEST_WEBSPACE+'/dir2', 0777)
            os.chmod(TEST_WEBSPACE+'/dir2/index.html', 0777)
        except OSError:
            pass
        try:
            shutil.rmtree(TEST_WEBSPACE)
        except OSError:
            pass


class GetViewsTest(BasePermissionsTest):

    def test_get_index_200(self):
        req = requests.get(HOST, timeout=2)
        self.assertEquals(req.status_code, 200)
        self.assertEquals(req.text, '<h1>index.html</h1>')

    def test_get_dir1_200(self):
        req = requests.get(HOST+'/dir1/', timeout=2)
        self.assertEquals(req.status_code, 200)
        self.assertEquals(req.text, '<h1>dir1/welcome.html</h1>')

    def test_get_page1_200(self):
        req = requests.get(HOST+'/dir1/page1.html', timeout=2)
        self.assertEquals(req.status_code, 200)
        self.assertEquals(req.text, '<h1>dir1/page1.html</h1>')

    def test_get_page2_403(self):
        req = requests.get(HOST+'/dir1/page2.html', timeout=2)
        self.assertEquals(req.status_code, 403)

    def test_get_dir2_403(self):
        req = requests.get(HOST+'/dir2/', timeout=2)
        self.assertEquals(req.status_code, 403)

    def test_get_none_404(self):
        req = requests.get(HOST+'/none.html', timeout=2)
        self.assertEquals(req.status_code, 404)


class HeadViewsTest(BasePermissionsTest):

    def test_head_index_200(self):
        req = requests.head(HOST, timeout=2)
        self.assertEquals(req.status_code, 200)

    def test_head_dir1_200(self):
        req = requests.head(HOST+'/dir1/', timeout=2)
        self.assertEquals(req.status_code, 200)

    def test_head_page1_200(self):
        req = requests.head(HOST+'/dir1/page1.html', timeout=2)
        self.assertEquals(req.status_code, 200)

    def test_head_page2_403(self):
        req = requests.head(HOST+'/dir1/page2.html', timeout=2)
        self.assertEquals(req.status_code, 403)

    def test_head_dir2_403(self):
        req = requests.head(HOST+'/dir2/', timeout=2)
        self.assertEquals(req.status_code, 403)

    def test_head_none_404(self):
        req = requests.head(HOST+'/none.html', timeout=2)
        self.assertEquals(req.status_code, 404)


class MethodExceptionTest(BasePermissionsTest):

    def test_post_index_501(self):
        req = requests.post(HOST, timeout=2)
        self.assertEquals(req.status_code, 501)

    def test_put_index_501(self):
        req = requests.put(HOST, timeout=2)
        self.assertEquals(req.status_code, 501)

    def test_patch_index_501(self):
        req = requests.patch(HOST, timeout=2)
        self.assertEquals(req.status_code, 501)

    def test_delete_index_501(self):
        req = requests.delete(HOST, timeout=2)
        self.assertEquals(req.status_code, 501)

    def test_options_index_501(self):
        req = requests.options(HOST, timeout=2)
        self.assertEquals(req.status_code, 501)

    def test_trace_index_501(self):
        req = requests.request("TRACE", HOST, timeout=2)
        self.assertEquals(req.status_code, 501)

    def test_trash_index_501(self):
        req = requests.request("TRASH", HOST, timeout=2)
        self.assertEquals(req.status_code, 501)
